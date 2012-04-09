#pragma once

#include "MoveIncludes.h"

namespace Move
{
	template <class D> 
	class VectorFunctor
	{
		public:
		virtual D operator()(std::vector<D> x)=0;  // call using operator
	};

	template <class D, class TClass> 
	class SpecVectorFunctor : public VectorFunctor<D>
	{
		private:
		D (TClass::*fpt)(std::vector<D>);   // pointer to member function
		TClass* pt2Object;                  // pointer to object

		public:

		// constructor - takes pointer to an object and pointer to a member and stores
		// them in two private variables
		SpecVectorFunctor(TClass* _pt2Object, D(TClass::*_fpt)(std::vector<D>))
			{ pt2Object = _pt2Object;  fpt=_fpt; };

		// override operator "()"
		virtual D operator()(std::vector<D> x)
		{ return (*pt2Object.*fpt)(x);};              // execute member function
	};
}