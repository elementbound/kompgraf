#ifndef _H_SUBDIV_OPERATOR_H_
#define _H_SUBDIV_OPERATOR_H_

#include "model.h"

class subdivOperator {
	public: 
		virtual model operator()(const model&);
};

class sierpinskiSubdivOperator {
	public:
		model operator()(const model&);
};

class loopSubdivOperator : public subdivOperator {
	public:
		model operator()(const model&);
};

#endif