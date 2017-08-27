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


static std::shared_ptr<Carbon::Node> nistTestFrequency(std::vector<std::shared_ptr<Carbon::Node>>& node) {
	if (node.size() == 1) {
		if (node[0]->GetNodeType() == Carbon::NodeType::Bits) {
			auto& l = reinterpret_cast<Carbon::NodeBits&>(*node[0]);
			auto& bits = l.Value;
			auto nistBitSequence = Convert(bits);
			Nist::Parameters testParameters;
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
			Nist::Parameters testParameters;
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
			Nist::Parameters testParameters;
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
			Nist::Parameters testParameters;
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
			Nist::Parameters testParameters;
			Nist::Results testResults;
			Nist::Test test(&nistBitSequence, &testParameters, &testResults);
			test.RunTestRank();
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.rank.p_value));
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.rank.F_30));
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.rank.F_31));
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.rank.F_32));
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.rank.N));
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.rank.chi_squared));
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.rank.p_30));
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.rank.p_31));
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.rank.p_32));
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
			Nist::Parameters testParameters;
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
			test.RunTestSerial();
			auto objResult = std::make_shared<Carbon::NodeObject>();
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.serial.p_value1));
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.serial.p_value2));
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.serial.del1));
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.serial.del2));
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.serial.psim0));
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.serial.psim1));
			objResult->Map.insert_or_assign("p_value", std::make_shared<Carbon::NodeFloat>(testResults.serial.psim2));
			return objResult;
		}
		else throw Carbon::ExecutorRuntimeException("nist test function only works on binary sequence");
	}
	else throw Carbon::ExecutorRuntimeException("nist test function needs 1 parameter");
}


void RegisterNistTestFunctions(Carbon::Executor& executor)
{
	executor.RegisterNativeFunction("nistFrequency", nistTestFrequency, true);
	executor.RegisterNativeFunction("nistBlockFrequency", nistTestBlockFrequency, true);
	executor.RegisterNativeFunction("nistRuns", nistTestRuns, true);
	executor.RegisterNativeFunction("nistLongestRunOfOnes", nistTestLongestRunOfOnes, true);
	executor.RegisterNativeFunction("nistRank", nistTestRank, true);
	executor.RegisterNativeFunction("nistSerial", nistTestSerial, true);
}
