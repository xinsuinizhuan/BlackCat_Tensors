/*
 * NeuralNetwork.cu
 *
 *  Created on: Jan 28, 2018
 *      Author: joseph
 */

#ifndef NEURALNETWORK_CU_
#define NEURALNETWORK_CU_

#include "Layer.cu"
#include <tuple>
#include <iostream>
#include <fstream>
#include <cstdio>


namespace BC {

template<class... set>
using LayerSet = std::tuple<set...>;


template<class... layers>
struct NeuralNetwork {
	static constexpr int length = sizeof...(layers);
	template<int a, int b> struct min { static constexpr int value = a < b ? a : b; };
	template<int a, int b> struct max { static constexpr int value = a > b ? a : b; };


	LayerSet<layers...> network;
	NeuralNetwork(layers... l) : network(std::make_tuple(l...)) {}

	template<class T> vec forwardPropagation	(vec_expr<T,ml>  param) 	   { return fp  <0, length>(param); }
	template<class T> vec forwardPropagation_express(vec_expr<T,ml>  param) 	   { return fpx <0, length>(param); }
	template<class T> vec backPropagation		(vec_expr<T,ml>  param) 	   { return bp  <length, 0>(param); }
	template<class T> vec backPropagationThroughTime(vec_expr<T,ml>  param) 	   { return bptt<length, 0>(param); }


	void updateWeights()   { update<0, length>(); }
	void clearBPStorage()  { clearStoredGradients<0,length>(); }

	void write(std::ofstream& os) {
		writeLayers<0, length>(os);
	}
	void read(std::ifstream& is) {
		readLayers<0, length>(is);
	}
	//This is just the iterator
private:

	template<int start, int end, class T, class ML> vec fp  (vec_expr<T,ML> param) {
		if (start != end)
			return fp  <min<start + 1, end>::value, end, T, ML>(std::get<min<start, end -1>::value>(network).forwardPropagation(param));
		else
			return param;
	}
	template<int start, int end, class T, class ML> vec fpx  (vec_expr<T,ML> param) {
		if (start != end)
			return fpx  <min<start + 1, end>::value, end, T, ML>(std::get<min<start, end -1>::value>(network).forwardPropagation_express(param));
		else
			return param;
	}
	template<int start, int end, class T, class ML> vec bp  (vec_expr<T,ML> param) {
		if (start > end)
			return bp  <max<start - 1, end>::value, end, T, ML>(std::get<max<start - 1, end>::value>(network).backPropagation(param));
		else
			return param;
	}
	template<int start, int end, class T, class ML> vec bptt  (vec_expr<T,ML> param) {
		if (start > end)
			return bptt  <max<start - 1, end>::value, end, T, ML>(std::get<max<start - 1, end>::value>(network).backPropagationThroughTime(param));
		else
			return param;
	}
	template<int start, int end> void update  () {
		if (start != end) {
			std::get<min<start, end -1>::value>(network).updateWeights();
			update<min<start + 1, end>::value, end>();
		}
	}
	template<int start, int end> void clearStoredGradients  () {

		if (start != end) {
			std::get<min<start, end -1>::value>(network).clearBPStorage();
			clearStoredGradients<min<start + 1, end>::value, end>();
		}
	}

	template<int start, int end> void writeLayers(std::ofstream& os) {

		if (start != end) {
			std::get<min<start, end -1>::value>(network).write(os);
			writeLayers<min<start + 1, end>::value, end>(os);
		}
	}
	template<int start, int end> void readLayers(std::ifstream& is) {

		if (start != end) {
			std::get<min<start, end -1>::value>(network).read(is);
			readLayers<min<start + 1, end>::value, end>(is);
		}
	}

};

template<class... layers>
static auto generateNetwork(layers&... l) {
	return NeuralNetwork<layers...>(l...);
}



}



#endif /* NEURALNETWORK_CU_ */
