/** @file CVTraits.hpp
	Contains the add_cv_pod_t and remove_cv_pod_t helper types. */
#ifndef __libcr_util_cvtraits_hpp_defined
#define __libcr_util_cvtraits_hpp_defined

#include "../mt/ConditionVariable.hpp"
#include "../sync/ConditionVariable.hpp"

namespace cr::util
{
	template<class ConditionVariable>
	/** Helper type for getting the non-POD version of a condition variable. */
	struct RemoveCVPOD {};
	template<class ConditionVariable>
	/** Helper type for getting the POD version of a condition variable. */
	struct AddCVPOD {};

	template<> struct RemoveCVPOD<mt::PODConditionVariable> { typedef mt::ConditionVariable type; };
	template<> struct AddCVPOD<mt::PODConditionVariable> { typedef mt::PODConditionVariable type; };
	template<> struct RemoveCVPOD<mt::ConditionVariable> { typedef mt::ConditionVariable type; };
	template<> struct AddCVPOD<mt::ConditionVariable> { typedef mt::PODConditionVariable type; };
	template<> struct RemoveCVPOD<mt::PODFIFOConditionVariable> { typedef mt::FIFOConditionVariable type; };
	template<> struct AddCVPOD<mt::PODFIFOConditionVariable> { typedef mt::PODFIFOConditionVariable type; };
	template<> struct RemoveCVPOD<mt::FIFOConditionVariable> { typedef mt::FIFOConditionVariable type; };
	template<> struct AddCVPOD<mt::FIFOConditionVariable> { typedef mt::PODFIFOConditionVariable type; };

	template<> struct RemoveCVPOD<sync::PODConditionVariable> { typedef sync::ConditionVariable type; };
	template<> struct AddCVPOD<sync::PODConditionVariable> { typedef sync::PODConditionVariable type; };
	template<> struct RemoveCVPOD<sync::ConditionVariable> { typedef sync::ConditionVariable type; };
	template<> struct AddCVPOD<sync::ConditionVariable> { typedef sync::PODConditionVariable type; };
	template<> struct RemoveCVPOD<sync::PODFIFOConditionVariable> { typedef sync::FIFOConditionVariable type; };
	template<> struct AddCVPOD<sync::PODFIFOConditionVariable> { typedef sync::PODFIFOConditionVariable type; };
	template<> struct RemoveCVPOD<sync::FIFOConditionVariable> { typedef sync::FIFOConditionVariable type; };
	template<> struct AddCVPOD<sync::FIFOConditionVariable> { typedef sync::PODFIFOConditionVariable type; };

	template<class ConditionVariable>
	/** Gets the non-POD version of a condition variable. */
	using remove_cv_pod_t = typename RemoveCVPOD<ConditionVariable>::type;
	template<class ConditionVariable>
	/** Gets the POD version of a condition variable. */
	using add_cv_pod_t = typename AddCVPOD<ConditionVariable>::type;
}

#endif