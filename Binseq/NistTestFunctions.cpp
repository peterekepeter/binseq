#include "NistTestFunctions.h"
#include <LibNistSts/common/stat_fncs.h>
#include <LibNistSts/Test.h>

static Nist::BitSequence Convert(binseq::bit_sequence& bits)
{
	auto size = bits.size();
	Nist::BitSequence bitSequence(bits.size());
	// todo optimize this
	for (unsigned long long i = 0; i<size; i++)
	{
		bitSequence[i] = bits[i];
	}
	return bitSequence;
}

static bool nistSettingsFastAuto = true;
static bool nistSettingsFast = false;


Nist::Parameters GetTestParameters(Nist::BitSequence& nistBitSequence)
{
	Nist::Parameters params;
	if (nistSettingsFastAuto) {
		// chose fast algorithm if sequence for longer sequences
		if (nistBitSequence.GetN() > 10000) {
			params.fast = true;
		} else {
			params.fast = false;
		}
	}
	else {
		params.fast = nistSettingsFast;
	}
	return params;
}

struct BlockLengthAndVersionResult
{
	bool foundBlockLength;
	size_t blockLength;
	bool foundVersion;
	std::string version;
};

BlockLengthAndVersionResult ParseBlockLengthAndVersion(std::vector<std::shared_ptr<Carbon::Node>>& node)
{
	BlockLengthAndVersionResult result;
	// init
	result.foundBlockLength = false;
	result.foundVersion = false;
	result.blockLength = 0;
	result.version = "v1";
	// serach params, param 0 must be bits, skip that
	for (int i=1; i<node.size(); i++)
	{
		auto& arg = node[i];
		// based on parameter type
		switch (arg->GetNodeType())
		{
		case Carbon::NodeType::Integer:
			if (!result.foundBlockLength)
			{
				auto value = reinterpret_cast<Carbon::NodeInteger&>(*arg).Value;
				if (value<0)
				{
					throw Carbon::ExecutorRuntimeException("block length should be a positive integer");
				}
				result.blockLength = value;
				result.foundBlockLength = true;
			} else
			{
				throw Carbon::ExecutorRuntimeException("function only accepts one optional integer parameter which is the block length");
			}
			break;
		case Carbon::NodeType::String:
			if (!result.foundVersion)
			{
				auto value = reinterpret_cast<Carbon::NodeString&>(*arg).Value;

				if (value.length()>3 || value.length()<2 || value[0] != 'v')
				{
					throw Carbon::ExecutorRuntimeException("version should be a string like v1, v2, v3 and so on");
				}
				result.version = value;
				result.foundVersion = true;
			}
			else
			{
				throw Carbon::ExecutorRuntimeException("function only accepts one optional integer parameter which is the block length");
			}
			break;
		default:
			throw Carbon::ExecutorRuntimeException("wrong parameter format, function expects optional block length and version (an integer and or string like \"v1\", \"v2\" and so on)");
		}
	}
	return result;
}

static std::shared_ptr<Carbon::Node> nistTestBlockFrequency(std::vector<std::shared_ptr<Carbon::Node>>& node); // forward

static std::shared_ptr<Carbon::Node> nistTestFrequency(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() >= 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto info = ParseBlockLengthAndVersion(node);
			// in case of this frequency test, in case block length is provided
			// it's safe to assume that user wants the nistTestBlockFrequency instead
			if (info.foundBlockLength)
			{
				// redirect
				return nistTestBlockFrequency(node);
			}
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			if (info.foundVersion)
			{
				// auto
				if (info.version == "v1")		Frequency(test);
				else if (info.version == "v2")	Frequency2(test);
				else if (info.version == "v3")	Frequency3(test);
				else if (info.version == "v4")	Frequency4(test);
				else throw Carbon::ExecutorRuntimeException("Frequency test version not found, possible values are v1, v2, v3, v4");
			}
			else
			{
				// based on speed setting 
				test.RunTestFrequency();
			}			
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.frequency.p_value));
			objResult->Map.insert_or_assign("sum", std::make_shared<Carbon::NodeFloat>(testResults.frequency.sum));
			objResult->Map.insert_or_assign("sum_n", std::make_shared<Carbon::NodeFloat>(testResults.frequency.sum_n));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs at least 1 parameter");
}



static std::shared_ptr<Carbon::Node> nistTestBlockFrequency(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() >= 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto info = ParseBlockLengthAndVersion(node);
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			if (info.foundBlockLength)
			{
				testParameters.blockFrequencyBlockLength = info.blockLength;
			}
			// check for paramter
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults); 
			// if provided version
			if (info.foundVersion)
			{
				// auto
				if (info.version == "v1")		BlockFrequency(test);
				else if (info.version == "v2")	BlockFrequency2(test);
				else if (info.version == "v3")	BlockFrequency3(test);
				else if (info.version == "v4")	BlockFrequency4(test);
				else throw Carbon::ExecutorRuntimeException("Block frequency test version not found, possible values are v1, v2, v3, v4");
			}
			else
			{
				// based on speed setting 
				test.RunTestBlockFrequency();
			}
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.blockfrequency.p_value));
			objResult->Map.insert_or_assign("chi_squared", std::make_shared<Carbon::NodeFloat>(testResults.blockfrequency.chi_squared));
			objResult->Map.insert_or_assign("block_length", std::make_shared<Carbon::NodeInteger>(testParameters.blockFrequencyBlockLength));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs at least 1 parameter");
}


static std::shared_ptr<Carbon::Node> nistTestRuns(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() >= 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto info = ParseBlockLengthAndVersion(node);
			if (info.foundBlockLength)
			{
				throw Carbon::ExecutorRuntimeException("Runs test does not need block length");
			}
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			// if provided version
			if (info.foundVersion)
			{
				// auto
				if (info.version == "v1")		Runs(test);
				else if (info.version == "v2")	Runs2(test);
				else if (info.version == "v3")	Runs3(test);
				else if (info.version == "v4")	Runs4(test);
				else throw Carbon::ExecutorRuntimeException("Runs test version not found, possible values are v1, v2, v3, v4");
			}
			else
			{
				// based on speed setting 
				test.RunTestRuns();
			}
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.runs.p_value));
			objResult->Map.insert_or_assign("V", std::make_shared<Carbon::NodeFloat>(testResults.runs.V));
			objResult->Map.insert_or_assign("erfc_arg", std::make_shared<Carbon::NodeFloat>(testResults.runs.erfc_arg));
			objResult->Map.insert_or_assign("pi", std::make_shared<Carbon::NodeFloat>(testResults.runs.pi));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs at least 1 parameter");
}

static std::shared_ptr<Carbon::Node> nistTestLongestRunOfOnes(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() >= 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto info = ParseBlockLengthAndVersion(node);
			if (info.foundBlockLength)
			{
				throw Carbon::ExecutorRuntimeException("LongestRunOfOnes test does not need block length");
			}
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			// if provided version
			if (info.foundVersion)
			{
				// auto
				if (info.version == "v1")		LongestRunOfOnes(test);
				else if (info.version == "v2")	LongestRunOfOnes2(test);
				else if (info.version == "v3")	LongestRunOfOnes3(test);
				else throw Carbon::ExecutorRuntimeException("LongestRunOfOnes test version not found, possible values are v1, v2, v3");
			}
			else
			{
				// based on speed setting 
				test.RunTestLongestRunOfOnes();
			}
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.longestrunofones.p_value));
			objResult->Map.insert_or_assign("M", std::make_shared<Carbon::NodeFloat>(testResults.longestrunofones.M));
			objResult->Map.insert_or_assign("N", std::make_shared<Carbon::NodeFloat>(testResults.longestrunofones.N));
			objResult->Map.insert_or_assign("chi2", std::make_shared<Carbon::NodeFloat>(testResults.longestrunofones.chi2));
			auto arrayResult = std::make_shared<Carbon::NodeArray>(7);
			for (int i=0; i<7; i++)
			{
				arrayResult->Vector[i] = std::make_shared<Carbon::NodeInteger>(testResults.longestrunofones.nu[i]);
			}
			objResult->Map.insert_or_assign("nu", arrayResult);
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs at least 1 parameter");
}

static std::shared_ptr<Carbon::Node> nistTestRank(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() >= 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto info = ParseBlockLengthAndVersion(node);
			if (info.foundBlockLength)
			{
				throw Carbon::ExecutorRuntimeException("Rank test does not need block length");
			}
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			// if provided version
			if (info.foundVersion)
			{
				// auto
				if (info.version == "v1")		Rank(test);
				else if (info.version == "v2")	Rank2(test);
				else throw Carbon::ExecutorRuntimeException("Rank test version not found, possible values are v1, v2");
			}
			else
			{
				// based on speed setting 
				test.RunTestRank();
			}
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.rank.p_value));
			objResult->Map.insert_or_assign("F_30", std::make_shared<Carbon::NodeFloat>(testResults.rank.F_30));
			objResult->Map.insert_or_assign("F_31", std::make_shared<Carbon::NodeFloat>(testResults.rank.F_31));
			objResult->Map.insert_or_assign("F_32", std::make_shared<Carbon::NodeFloat>(testResults.rank.F_32));
			objResult->Map.insert_or_assign("N", std::make_shared<Carbon::NodeFloat>(testResults.rank.N));
			objResult->Map.insert_or_assign("chi_squared", std::make_shared<Carbon::NodeFloat>(testResults.rank.chi_squared));
			objResult->Map.insert_or_assign("p_30", std::make_shared<Carbon::NodeFloat>(testResults.rank.p_30));
			objResult->Map.insert_or_assign("p_31", std::make_shared<Carbon::NodeFloat>(testResults.rank.p_31));
			objResult->Map.insert_or_assign("p_32", std::make_shared<Carbon::NodeFloat>(testResults.rank.p_32));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs at least 1 parameter");
}

static std::shared_ptr<Carbon::Node> nistTestSerial(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() >= 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto info = ParseBlockLengthAndVersion(node);
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			// check for paramter
			if (info.foundBlockLength)
			{
				testParameters.serialBlockLength = info.blockLength;
			}
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			// run
			// if provided version
			if (info.foundVersion)
			{
				// auto
				if (info.version == "v1")		Serial(test);
				else if (info.version == "v2")	Serial2(test);
				else if (info.version == "v4")	Serial4(test);
				else throw Carbon::ExecutorRuntimeException("Serial test version not found, possible values are v1, v2, v4");
			}
			else
			{
				// based on speed setting 
				test.RunTestSerial();
			}
			// build dynamic object to return results
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value1", std::make_shared<Carbon::NodeFloat>(testResults.serial.p_value1));
			objResult->Map.insert_or_assign("p_value2", std::make_shared<Carbon::NodeFloat>(testResults.serial.p_value2));
			objResult->Map.insert_or_assign("del1", std::make_shared<Carbon::NodeFloat>(testResults.serial.del1));
			objResult->Map.insert_or_assign("del2", std::make_shared<Carbon::NodeFloat>(testResults.serial.del2));
			objResult->Map.insert_or_assign("psim0", std::make_shared<Carbon::NodeFloat>(testResults.serial.psim0));
			objResult->Map.insert_or_assign("psim1", std::make_shared<Carbon::NodeFloat>(testResults.serial.psim1));
			objResult->Map.insert_or_assign("psim2", std::make_shared<Carbon::NodeFloat>(testResults.serial.psim2));
			objResult->Map.insert_or_assign("block_length", std::make_shared<Carbon::NodeInteger>(testParameters.serialBlockLength));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs at least 1 parameter");
}


static std::shared_ptr<Carbon::Node> nistTestNonOverlappingTemplateMatchings(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() >= 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto info = ParseBlockLengthAndVersion(node);
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			// check for paramter
			if (info.foundBlockLength)
			{
				testParameters.nonOverlappingTemplateBlockLength = info.blockLength;
			}
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			// if provided version
			if (info.foundVersion)
			{
				// auto
				if (info.version == "v1")		NonOverlappingTemplateMatchings(test);
				else if (info.version == "v2")	NonOverlappingTemplateMatchings2(test);
				else if (info.version == "v4")	NonOverlappingTemplateMatchings4(test);
				else throw Carbon::ExecutorRuntimeException("NonOverlappingTemplateMatchings test version not found, possible values are v1, v2, v4");
			}
			else
			{
				// based on speed setting 
				test.RunTestNonOverlappingTemplateMatchings();
			}
			auto objResult = std::make_shared<Carbon::NodeObject>();

			auto arrayPValue = std::make_shared<Carbon::NodeArray>();
			for (auto x : testResults.nonoverlapping.p_value) {
				arrayPValue->Vector.push_back(std::make_shared<Carbon::NodeFloat>(x));
			}
			objResult->Map.insert_or_assign("p_value", arrayPValue);

			auto arrayW = std::make_shared<Carbon::NodeArray>();
			for (auto x : testResults.nonoverlapping.W) {
				arrayPValue->Vector.push_back(std::make_shared<Carbon::NodeInteger>(x));
			}
			objResult->Map.insert_or_assign("W", arrayW);

			objResult->Map.insert_or_assign("templates", std::make_shared<Carbon::NodeInteger>(testResults.nonoverlapping.templates));
			objResult->Map.insert_or_assign("block_length", std::make_shared<Carbon::NodeInteger>(testParameters.nonOverlappingTemplateBlockLength));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs at least 1 parameter");
}


static std::shared_ptr<Carbon::Node> nistTestOverlappingTemplateMatchings(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() >= 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto info = ParseBlockLengthAndVersion(node);
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			// check for paramter
			if (info.foundBlockLength)
			{
				testParameters.overlappingTemplateBlockLength = info.blockLength;
			}
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			// if provided version
			if (info.foundVersion)
			{
				// auto
				if (info.version == "v1")		OverlappingTemplateMatchings(test);
				else if (info.version == "v2")	OverlappingTemplateMatchings2(test);
				else if (info.version == "v3")	OverlappingTemplateMatchings3(test);
				else if (info.version == "v4")	OverlappingTemplateMatchings4(test);
				else throw Carbon::ExecutorRuntimeException("OverlappingTemplateMatchings test version not found, possible values are v1, v2, v3, v4");
			}
			else
			{
				// based on speed setting 
				test.RunTestOverlappingTemplateMatchings();
			}
			auto objResult = std::make_shared<Carbon::NodeObject>();

			auto arrayPValue = std::make_shared<Carbon::NodeArray>();
			for (auto x : testResults.overlapping.nu) {
				arrayPValue->Vector.push_back(std::make_shared<Carbon::NodeInteger>(x));
			}
			objResult->Map.insert_or_assign("nu", arrayPValue);

			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeInteger>(testResults.overlapping.p_value));
			objResult->Map.insert_or_assign("chi2", std::make_shared<Carbon::NodeInteger>(testResults.overlapping.chi2));
			objResult->Map.insert_or_assign("block_length", std::make_shared<Carbon::NodeInteger>(testParameters.overlappingTemplateBlockLength));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs at least 1 parameter");
}



static std::shared_ptr<Carbon::Node> nistTestUniversal(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() >= 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto info = ParseBlockLengthAndVersion(node);
			if (info.foundBlockLength)
			{
				throw Carbon::ExecutorRuntimeException("Universal test does not need block length");
			}
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			// if provided version
			if (info.foundVersion)
			{
				// auto
				if (info.version == "v1")		Universal(test);
				else if (info.version == "v2")	Universal2(test);
				else throw Carbon::ExecutorRuntimeException("Universal test version not found, possible values are v1, v2");
			}
			else
			{
				// based on speed setting 
				test.RunTestUniversal();
			}
			test.RunTestUniversal();
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.universal.p_value));
			objResult->Map.insert_or_assign("phi", std::make_shared<Carbon::NodeFloat>(testResults.universal.phi));
			objResult->Map.insert_or_assign("sum", std::make_shared<Carbon::NodeFloat>(testResults.universal.sum));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs at least 1 parameter");
}


static std::shared_ptr<Carbon::Node> nistTestApproximateEntropy(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() >= 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto info = ParseBlockLengthAndVersion(node);
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			// check for paramter
			if (info.foundBlockLength)
			{
				testParameters.approximateEntropyBlockLength = info.blockLength;
			}
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			// if provided version
			if (info.foundVersion)
			{
				// auto
				if (info.version == "v1")		ApproximateEntropy(test);
				else if (info.version == "v2")	ApproximateEntropy2(test);
				else if (info.version == "v4")	ApproximateEntropy4(test);
				else throw Carbon::ExecutorRuntimeException("Universal test version not found, possible values are v1, v2, v4");
			}
			else
			{
				// based on speed setting 
				test.RunTestApproximateEntropy();
			}
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.approximate_entropy.p_value));
			auto arrayP = std::make_shared<Carbon::NodeArray>();
			for (auto x : testResults.approximate_entropy.P) {
				arrayP->Vector.push_back(std::make_shared<Carbon::NodeInteger>(x));
			}
			objResult->Map.insert_or_assign("P", arrayP);
			auto arrayApEn = std::make_shared<Carbon::NodeArray>();
			for (auto x : testResults.approximate_entropy.ApEn) {
				arrayP->Vector.push_back(std::make_shared<Carbon::NodeFloat>(x));
			}
			objResult->Map.insert_or_assign("ApEn", arrayApEn);
			objResult->Map.insert_or_assign("pp", std::make_shared<Carbon::NodeInteger>(testResults.approximate_entropy.pp));
			objResult->Map.insert_or_assign("chi_squared", std::make_shared<Carbon::NodeFloat>(testResults.approximate_entropy.chi_squared));
			objResult->Map.insert_or_assign("block_length", std::make_shared<Carbon::NodeInteger>(testParameters.approximateEntropyBlockLength));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs at least 1 parameter");
}


static std::shared_ptr<Carbon::Node> nistTestCusum(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() >= 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto info = ParseBlockLengthAndVersion(node);
			if (info.foundBlockLength)
			{
				throw Carbon::ExecutorRuntimeException("CumulativeSums test does not need block length");
			}
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			// if provided version
			if (info.foundVersion)
			{
				// auto
				if (info.version == "v1")		CumulativeSums(test);
				else if (info.version == "v2")	CumulativeSums2(test);
				else if (info.version == "v3")	CumulativeSums3(test);
				else throw Carbon::ExecutorRuntimeException("CumulativeSums test version not found, possible values are v1, v2, v3");
			}
			else
			{
				// based on speed setting 
				test.RunTestCumulativeSums();
			}
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_valueA", std::make_shared<Carbon::NodeFloat>(testResults.cusum.p_valueA));
			objResult->Map.insert_or_assign("p_valueB", std::make_shared<Carbon::NodeFloat>(testResults.cusum.p_valueB));
			objResult->Map.insert_or_assign("sum1A", std::make_shared<Carbon::NodeFloat>(testResults.cusum.sum1A));
			objResult->Map.insert_or_assign("sum2A", std::make_shared<Carbon::NodeFloat>(testResults.cusum.sum2A));
			objResult->Map.insert_or_assign("sum1B", std::make_shared<Carbon::NodeFloat>(testResults.cusum.sum1B));
			objResult->Map.insert_or_assign("sum2B", std::make_shared<Carbon::NodeFloat>(testResults.cusum.sum2B));
			objResult->Map.insert_or_assign("z", std::make_shared<Carbon::NodeInteger>(testResults.cusum.z));
			objResult->Map.insert_or_assign("zrev", std::make_shared<Carbon::NodeInteger>(testResults.cusum.zrev));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs at least 1 parameter");
}


static std::shared_ptr<Carbon::Node> nistTestRandomExcursion(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() >= 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto info = ParseBlockLengthAndVersion(node);
			if (info.foundBlockLength)
			{
				throw Carbon::ExecutorRuntimeException("RandomExcursions test does not need block length");
			}
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			// if provided version
			if (info.foundVersion)
			{
				// auto
				if (info.version == "v1")		RandomExcursions(test);
				else if (info.version == "v2")	RandomExcursions2(test);
				else throw Carbon::ExecutorRuntimeException("RandomExcursions test version not found, possible values are v1, v2");
			}
			else
			{
				// based on speed setting 
				test.RunTestRandomExcursions();
			}
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("valid", std::make_shared<Carbon::NodeBit>(testResults.random_excursion.valid));
			auto objArray = std::make_shared<Carbon::NodeArray>();
			for (int i = 0; i < testResults.random_excursion.J.size(); i++){
				auto item = std::make_shared<Carbon::NodeObject>();
				item->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.random_excursion.p_value[i]));
				item->Map.insert_or_assign("sum", std::make_shared<Carbon::NodeFloat>(testResults.random_excursion.sum[i]));
				item->Map.insert_or_assign("J", std::make_shared<Carbon::NodeInteger>(testResults.random_excursion.J[i]));
				item->Map.insert_or_assign("x", std::make_shared<Carbon::NodeInteger>(testResults.random_excursion.x[i]));
				objArray->Vector.push_back(item);
			}
			objResult->Map.insert_or_assign("results", objArray);
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs at least 1 parameter");
}

static std::shared_ptr<Carbon::Node> nistTestRandomExcursionVariant(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() >= 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto info = ParseBlockLengthAndVersion(node);
			if (info.foundBlockLength)
			{
				throw Carbon::ExecutorRuntimeException("RandomExcursionsVariant test does not need block length");
			}
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			// if provided version
			if (info.foundVersion)
			{
				// auto
				if (info.version == "v1")		RandomExcursionsVariant(test);
				else if (info.version == "v2")	RandomExcursionsVariant2(test);
				else throw Carbon::ExecutorRuntimeException("RandomExcursionsVariant test version not found, possible values are v1, v2");
			}
			else
			{
				// based on speed setting 
				test.RunTestRandomExcursionsVariant();
			}
			test.RunTestRandomExcursionsVariant();
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("valid", std::make_shared<Carbon::NodeBit>(testResults.random_excursion_variant.valid));
			auto objArray = std::make_shared<Carbon::NodeArray>();
			for (int i = 0; i < testResults.random_excursion_variant.p_value.size(); i++) {
				auto item = std::make_shared<Carbon::NodeObject>();
				item->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.random_excursion_variant.p_value[i]));
				item->Map.insert_or_assign("count", std::make_shared<Carbon::NodeInteger>(testResults.random_excursion_variant.count[i]));
				item->Map.insert_or_assign("x", std::make_shared<Carbon::NodeInteger>(testResults.random_excursion_variant.x[i]));
				objArray->Vector.push_back(item);
			}
			objResult->Map.insert_or_assign("results", objArray);
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs at least 1 parameter");
}

// set test suite settings
static std::shared_ptr<Carbon::Node> nistSettings( std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() > 1) {
		throw Carbon::ExecutorRuntimeException("accepts only one string parameter");
	}
	if (node.size() == 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::String) {
			auto& node0 = reinterpret_cast<Carbon::NodeString&>(*node[0]);
			auto& str = node0.Value;
			if (str.find("fast") != std::string::npos) { nistSettingsFast = true; nistSettingsFastAuto = false; };
			if (str.find("slow") != std::string::npos) { nistSettingsFast = false; nistSettingsFastAuto = false; }
			if (str.find("auto") != std::string::npos) { nistSettingsFast = false; nistSettingsFastAuto = true;  }
		}
		else throw Carbon::ExecutorRuntimeException("parameter is not a string");
	}
	std::string acc = "";
	if (nistSettingsFastAuto) acc = "auto";
	else if (nistSettingsFast) {
		acc = "fast";
	}
	else {
		acc = "slow";
	}
	return std::make_shared<Carbon::NodeString>(acc);
}


static std::shared_ptr<Carbon::Node> nistTestLinearComplexity(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() >= 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto info = ParseBlockLengthAndVersion(node);
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			// check for paramter
			if (info.foundBlockLength)
			{
				testParameters.linearComplexitySequenceLength = info.blockLength;
			}
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			// if provided version
			if (info.foundVersion)
			{
				// auto
				if (info.version == "v1")		LinearComplexity(test);
				else if (info.version == "v2")	LinearComplexity2(test);
				else if (info.version == "v3")	LinearComplexity3(test);
				else throw Carbon::ExecutorRuntimeException("LinearComplexity test version not found, possible values are v1, v2, v3");
			}
			else
			{
				// based on speed setting 
				test.RunTestLinearComplexity();
			}
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.linear_complexity.p_value));
			objResult->Map.insert_or_assign("chi2", std::make_shared<Carbon::NodeFloat>(testResults.linear_complexity.chi2));
			objResult->Map.insert_or_assign("block_length", std::make_shared<Carbon::NodeFloat>(testParameters.linearComplexitySequenceLength));
			auto objArray = std::make_shared<Carbon::NodeArray>();
			for (int i = 0; i < testResults.linear_complexity.nu.size(); i++) {
				objArray->Vector.push_back(std::make_shared<Carbon::NodeInteger>(testResults.linear_complexity.nu[i]));
			}
			objResult->Map.insert_or_assign("nu", objArray);
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs at least 1 parameter");
}


static std::shared_ptr<Carbon::Node> nistTestDft(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() >= 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto info = ParseBlockLengthAndVersion(node);
			if (info.foundBlockLength)
			{
				throw Carbon::ExecutorRuntimeException("DiscreteFourierTransform test does not need block length");
			}
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			// if provided version
			if (info.foundVersion)
			{
				// auto
				if (info.version == "v1")		DiscreteFourierTransform(test);
				else if (info.version == "v2")	DiscreteFourierTransform2(test);
				else throw Carbon::ExecutorRuntimeException("DiscreteFourierTransform test version not found, possible values are v1, v2");
			}
			else
			{
				// based on speed setting 
				test.RunTestDiscreteFourierTransform();
			}
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.dft.p_value));
			objResult->Map.insert_or_assign("d", std::make_shared<Carbon::NodeFloat>(testResults.dft.d));
			objResult->Map.insert_or_assign("N_l", std::make_shared<Carbon::NodeFloat>(testResults.dft.N_l));
			objResult->Map.insert_or_assign("N_o", std::make_shared<Carbon::NodeFloat>(testResults.dft.N_o));
			objResult->Map.insert_or_assign("percentile", std::make_shared<Carbon::NodeFloat>(testResults.dft.percentile));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs at least 1 parameter");
}

void RegisterNistTestFunctions(Carbon::Executor& executor)
{
	// functions to register
	std::vector<std::pair<std::string, Carbon::native_function_ptr>> functions = {
		{ "nistFrequency", nistTestFrequency							},
		{ "nistRuns", nistTestRuns										},
		{ "nistLongestRunOfOnes", nistTestLongestRunOfOnes				},
		{ "nistRank", nistTestRank										},
		{ "nistSerial", nistTestSerial									},
		{ "nistSettings", nistSettings									},
		{ "nistUniversal", nistTestUniversal							},
		{ "nistApproximateEntropy", nistTestApproximateEntropy			},
		{ "nistCusum", nistTestCusum									},
		{ "nistRandomExcursion", nistTestRandomExcursion				},
		{ "nistRandomExcursionVariant", nistTestRandomExcursionVariant	},
		{ "nistLinearComplexity", nistTestLinearComplexity				},
		{ "nistDft", nistTestDft										}
	};
	// now actually register them
	for (auto& function : functions) {
		executor.RegisterNativeFunction(function.first.c_str(), function.second, false);
	}
}
