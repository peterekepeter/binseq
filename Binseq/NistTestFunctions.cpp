#include "NistTestFunctions.h"
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


static std::shared_ptr<Carbon::Node> nistTestFrequency(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() == 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			test.RunTestFrequency();			
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.frequency.p_value));
			objResult->Map.insert_or_assign("sum", std::make_shared<Carbon::NodeFloat>(testResults.frequency.sum));
			objResult->Map.insert_or_assign("sum_n", std::make_shared<Carbon::NodeFloat>(testResults.frequency.sum_n));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs 1 parameter");
}



static std::shared_ptr<Carbon::Node> nistTestBlockFrequency(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() >= 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			// check for paramter
			if (node.size()>=2)
			{
				if (node[1]->GetNodeType() == Carbon::NodeType::Integer)
				{
					auto& param = reinterpret_cast<Carbon::NodeInteger&>(*node[1]);
					testParameters.blockFrequencyBlockLength = param.Value;
				}
				else
				{
					throw Carbon::ExecutorRuntimeException("second parameter for test function may only be an integer");
				}
			}
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			test.RunTestBlockFrequency();
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.blockfrequency.p_value));
			objResult->Map.insert_or_assign("chi_squared", std::make_shared<Carbon::NodeFloat>(testResults.blockfrequency.chi_squared));
			objResult->Map.insert_or_assign("block_length", std::make_shared<Carbon::NodeInteger>(testParameters.blockFrequencyBlockLength));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs 1 parameter");
}


static std::shared_ptr<Carbon::Node> nistTestRuns(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() == 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			test.RunTestRuns();
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.runs.p_value));
			objResult->Map.insert_or_assign("V", std::make_shared<Carbon::NodeFloat>(testResults.runs.V));
			objResult->Map.insert_or_assign("erfc_arg", std::make_shared<Carbon::NodeFloat>(testResults.runs.erfc_arg));
			objResult->Map.insert_or_assign("pi", std::make_shared<Carbon::NodeFloat>(testResults.runs.pi));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs 1 parameter");
}

static std::shared_ptr<Carbon::Node> nistTestLongestRunOfOnes(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() == 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			test.RunTestLongestRunOfOnes();
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
	else throw Carbon::ExecutorRuntimeException("nist test function needs 1 parameter");
}

static std::shared_ptr<Carbon::Node> nistTestRank(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() == 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			test.RunTestRank();
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
	else throw Carbon::ExecutorRuntimeException("nist test function needs 1 parameter");
}

static std::shared_ptr<Carbon::Node> nistTestSerial(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() == 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			// check for paramter
			if (node.size() >= 2)
			{
				if (node[1]->GetNodeType() == Carbon::NodeType::Integer)
				{
					auto& param = reinterpret_cast<Carbon::NodeInteger&>(*node[1]);
					testParameters.serialBlockLength = param.Value;
				}
				else
				{
					throw Carbon::ExecutorRuntimeException("second parameter for test function may only be an integer");
				}
			}
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			// run
			test.RunTestSerial();
			// build dynamic object to return results
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value1", std::make_shared<Carbon::NodeFloat>(testResults.serial.p_value1));
			objResult->Map.insert_or_assign("p_value2", std::make_shared<Carbon::NodeFloat>(testResults.serial.p_value2));
			objResult->Map.insert_or_assign("del1", std::make_shared<Carbon::NodeFloat>(testResults.serial.del1));
			objResult->Map.insert_or_assign("del2", std::make_shared<Carbon::NodeFloat>(testResults.serial.del2));
			objResult->Map.insert_or_assign("psim0", std::make_shared<Carbon::NodeFloat>(testResults.serial.psim0));
			objResult->Map.insert_or_assign("psim1", std::make_shared<Carbon::NodeFloat>(testResults.serial.psim1));
			objResult->Map.insert_or_assign("psim2", std::make_shared<Carbon::NodeFloat>(testResults.serial.psim2));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs 1 parameter");
}


//TODO non overlapping

//TODO overlapping



static std::shared_ptr<Carbon::Node> nistTestUniversal(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() == 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			test.RunTestUniversal();
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.universal.p_value));
			objResult->Map.insert_or_assign("phi", std::make_shared<Carbon::NodeFloat>(testResults.universal.phi));
			objResult->Map.insert_or_assign("sum", std::make_shared<Carbon::NodeFloat>(testResults.universal.sum));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs 1 parameter");
}


static std::shared_ptr<Carbon::Node> nistTestApproximateEntropy(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() == 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			// check for paramter
			if (node.size() >= 2)
			{
				if (node[1]->GetNodeType() == Carbon::NodeType::Integer)
				{
					auto& param = reinterpret_cast<Carbon::NodeInteger&>(*node[1]);
					testParameters.approximateEntropyBlockLength = param.Value;
				}
				else
				{
					throw Carbon::ExecutorRuntimeException("second parameter for test function may only be an integer");
				}
			}
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			test.RunTestApproximateEntropy();
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
			objResult->Map.insert_or_assign("blockLength", std::make_shared<Carbon::NodeInteger>(testParameters.approximateEntropyBlockLength));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs 1 parameter");
}


static std::shared_ptr<Carbon::Node> nistTestCusum(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() == 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			test.RunTestCumulativeSums();
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
	else throw Carbon::ExecutorRuntimeException("nist test function needs 1 parameter");
}


static std::shared_ptr<Carbon::Node> nistTestRandomExcursion(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() == 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			test.RunTestRandomExcursions();
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
	else throw Carbon::ExecutorRuntimeException("nist test function needs 1 parameter");
}

static std::shared_ptr<Carbon::Node> nistTestRandomExcursionVariant(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() == 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
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
	else throw Carbon::ExecutorRuntimeException("nist test function needs 1 parameter");
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
	if (node.size() == 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			// check for paramter
			if (node.size() >= 2)
			{
				if (node[1]->GetNodeType() == Carbon::NodeType::Integer)
				{
					auto& param = reinterpret_cast<Carbon::NodeInteger&>(*node[1]);
					testParameters.linearComplexitySequenceLength = param.Value;
				}
				else
				{
					throw Carbon::ExecutorRuntimeException("second parameter for test function may only be an integer");
				}
			}
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			test.RunTestLinearComplexity();
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.linear_complexity.p_value));
			objResult->Map.insert_or_assign("chi2", std::make_shared<Carbon::NodeFloat>(testResults.linear_complexity.chi2));
			auto objArray = std::make_shared<Carbon::NodeArray>();
			for (int i = 0; i < testResults.linear_complexity.nu.size(); i++) {
				objArray->Vector.push_back(std::make_shared<Carbon::NodeInteger>(testResults.linear_complexity.nu[i]));
			}
			objResult->Map.insert_or_assign("nu", objArray);
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs 1 parameter");
}


static std::shared_ptr<Carbon::Node> nistTestDft(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() == 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters = GetTestParameters(nistBitSequence);
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			test.RunTestDiscreteFourierTransform();
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
	else throw Carbon::ExecutorRuntimeException("nist test function needs 1 parameter");
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
