#pragma once

#include <boost/preprocessor.hpp>

/*
https://stackoverflow.com/questions/5093460/how-to-convert-an-enum-type-variable-to-a-string
*/

// ADD_PARENTHESES_FOR_EACH_TUPLE_IN_SEQ implementation is taken from:
// http://lists.boost.org/boost-users/2012/09/76055.php
//
// This macro do the following:
// input:
//      (Element1, "Element 1 string repr", 2) (Element2) (Element3, "Element 3 string repr")
// output:
//      ((Element1, "Element 1 string repr", 2)) ((Element2)) ((Element3, "Element 3 string repr"))
#define HELPER1(...) ((__VA_ARGS__)) HELPER2
#define HELPER2(...) ((__VA_ARGS__)) HELPER1
#define HELPER1_END
#define HELPER2_END
#define ADD_PARENTHESES_FOR_EACH_TUPLE_IN_SEQ(sequence) BOOST_PP_CAT(HELPER1 sequence,_END)

// CREATE_ENUM_ELEMENT_IMPL works in the following way:
//  if (elementTuple.GetSize() == 4) {
//      GENERATE: elementTuple.GetElement(0) = elementTuple.GetElement(2)),
//  } else {
//      GENERATE: elementTuple.GetElement(0),
//  }
// Example 1:
//      CREATE_ENUM_ELEMENT_IMPL((Element1, "Element 1 string repr", 2, _))
//  generates:
//      Element1 = 2,
//
// Example 2:
//      CREATE_ENUM_ELEMENT_IMPL((Element2, _))
//  generates:
//      Element1,
#define CREATE_ENUM_ELEMENT_IMPL(elementTuple)                                          \
BOOST_PP_IF(BOOST_PP_EQUAL(BOOST_PP_TUPLE_SIZE(elementTuple), 4),                       \
    BOOST_PP_TUPLE_ELEM(0, elementTuple) = BOOST_PP_TUPLE_ELEM(2, elementTuple),        \
    BOOST_PP_TUPLE_ELEM(0, elementTuple)                                                \
),

// we have to add a dummy element at the end of a tuple in order to make
// BOOST_PP_TUPLE_ELEM macro work in case an initial tuple has only one element.
// if we have a tuple (Element1), BOOST_PP_TUPLE_ELEM(2, (Element1)) macro won't compile.
// It requires that a tuple with only one element looked like (Element1,).
// Unfortunately I couldn't find a way to make this transformation, so
// I just use BOOST_PP_TUPLE_PUSH_BACK macro to add a dummy element at the end
// of a tuple, in this case the initial tuple will look like (Element1, _) what
// makes it compatible with BOOST_PP_TUPLE_ELEM macro
#define CREATE_ENUM_ELEMENT(r, data, elementTuple)                                      \
    CREATE_ENUM_ELEMENT_IMPL(BOOST_PP_TUPLE_PUSH_BACK(elementTuple, _))

#define DEFINE_CASE_HAVING_ONLY_ENUM_ELEMENT_NAME(enumName, element)                                        \
    case enumName::element : return BOOST_PP_STRINGIZE(element);
#define DEFINE_CASE_HAVING_STRING_REPRESENTATION_FOR_ENUM_ELEMENT(enumName, element, stringRepresentation)  \
    case enumName::element : return stringRepresentation;

// GENERATE_CASE_FOR_SWITCH macro generates case for switch operator.
// Algorithm of working is the following
//  if (elementTuple.GetSize() == 1) {
//      DEFINE_CASE_HAVING_ONLY_ENUM_ELEMENT_NAME(enumName, elementTuple.GetElement(0))
//  } else {
//      DEFINE_CASE_HAVING_STRING_REPRESENTATION_FOR_ENUM_ELEMENT(enumName, elementTuple.GetElement(0), elementTuple.GetElement(1))
//  }
//
// Example 1:
//      GENERATE_CASE_FOR_SWITCH(_, EnumName, (Element1, "Element 1 string repr", 2))
//  generates:
//      case EnumName::Element1 : return "Element 1 string repr";
//
// Example 2:
//      GENERATE_CASE_FOR_SWITCH(_, EnumName, (Element2))
//  generates:
//      case EnumName::Element2 : return "Element2";
#define GENERATE_CASE_FOR_SWITCH(r, enumName, elementTuple)                                                                                                 \
    BOOST_PP_IF(BOOST_PP_EQUAL(BOOST_PP_TUPLE_SIZE(elementTuple), 1),                                                                                       \
        DEFINE_CASE_HAVING_ONLY_ENUM_ELEMENT_NAME(enumName, BOOST_PP_TUPLE_ELEM(0, elementTuple)),                                                          \
        DEFINE_CASE_HAVING_STRING_REPRESENTATION_FOR_ENUM_ELEMENT(enumName, BOOST_PP_TUPLE_ELEM(0, elementTuple), BOOST_PP_TUPLE_ELEM(1, elementTuple))     \
    )

// DEFINE_ENUM_CLASS_WITH_ToString_METHOD final macro witch do the job
#define DEFINE_ENUM_CLASS_WITH_ToString_METHOD(enumName, enumElements)          \
enum class enumName {                                                           \
    BOOST_PP_SEQ_FOR_EACH(                                                      \
        CREATE_ENUM_ELEMENT,                                                    \
        0,                                                                      \
        ADD_PARENTHESES_FOR_EACH_TUPLE_IN_SEQ(enumElements)                     \
    )                                                                           \
};                                                                              \
inline const char* ToString(const enumName element) {                           \
        switch (element) {                                                      \
            BOOST_PP_SEQ_FOR_EACH(                                              \
                GENERATE_CASE_FOR_SWITCH,                                       \
                enumName,                                                       \
                ADD_PARENTHESES_FOR_EACH_TUPLE_IN_SEQ(enumElements)             \
            )                                                                   \
            default: return "[Unknown " BOOST_PP_STRINGIZE(enumName) "]";       \
        }                                                                       \
}

//Implementation
//DEFINE_ENUM_CLASS_WITH_ToString_METHOD(Elements,
//(Element1)
//(Element2, "string representation for Element2 ")
//(Element3, "Element3 string representation", 1000)
//(Element4, "Element 4 string repr")
//(Element5, "Element5", 1005)
//(Element6, "Element6 ")
//(Element7)
//)

//Explanation of behind the scences

// Generates the following:
//      enum class Elements {
//          Element1, Element2, Element3 = 1000, Element4, Element5 = 1005, Element6,
//      };
//      inline const char* ToString(const Elements element) {
//          switch (element) {
//              case Elements::Element1: return "Element1";
//              case Elements::Element2: return "string representation for Element2 ";
//              case Elements::Element3: return "Element3 string representation";
//              case Elements::Element4: return "Element 4 string repr";
//              case Elements::Element5: return "Element5";
//              case Elements::Element6: return "Element6 ";
//              case Elements::Element7: return "Element7";
//              default: return "[Unknown " "Elements" "]";
//          }
//      }

//Usage
//int main() {
//	std::cout << ToString(Elements::Element1) << std::endl;
//	std::cout << ToString(Elements::Element2) << std::endl;
//	std::cout << ToString(Elements::Element3) << std::endl;
//	std::cout << ToString(Elements::Element4) << std::endl;
//	std::cout << ToString(Elements::Element5) << std::endl;
//	std::cout << ToString(Elements::Element6) << std::endl;
//	std::cout << ToString(Elements::Element7) << std::endl;
//
//	return 0;
//}
