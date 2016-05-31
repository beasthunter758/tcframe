#pragma once

#include <algorithm>
#include <queue>
#include <string>
#include <type_traits>
#include <vector>

#include "tcframe/io.hpp"
#include "tcframe/util.hpp"
#include "tcframe/variable.hpp"

using std::enable_if;
using std::forward;
using std::is_arithmetic;
using std::is_reference;
using std::is_same;
using std::queue;
using std::string;
using std::vector;

#define CONS(predicate) addConstraint(Constraint([=] {return predicate;}, #predicate))
#define CASE(...) addOfficialTestCase(OfficialTestCase([=] {__VA_ARGS__;}, #__VA_ARGS__))
#define SAMPLE_CASE(...) addSampleTestCase(__VA_ARGS__)
#define LINE(...) MagicLineIOSegmentBuilder(newLineIOSegment(), #__VA_ARGS__), __VA_ARGS__
#define LINES(...) (MagicLinesIOSegmentBuilder(newLinesIOSegment(), #__VA_ARGS__), __VA_ARGS__)
#define GRID(...) (MagicGridIOSegmentBuilder(newGridIOSegment(), #__VA_ARGS__), __VA_ARGS__)

#define SIZE_IMPL1(size) VectorSize{size}
#define SIZE_IMPL2(rows, columns) MatrixSize{rows, columns}
#define SIZE_WITH_COUNT(_1, _2, N, ...) SIZE_IMPL ## N
#define SIZE(...) SIZE_WITH_COUNT(__VA_ARGS__, 2, 1)(__VA_ARGS__)


namespace tcframe {

template<typename T>
using ScalarCompatible = typename enable_if<!is_reference<T>::value && (is_arithmetic<T>::value || is_same<string, T>::value)>::type;

template<typename T>
using NotScalarCompatible = typename enable_if<is_reference<T>::value || (!is_arithmetic<T>::value && !is_same<string, T>::value)>::type;

struct VectorSize {
    int size;
};

template<typename T>
struct VectorWithSize {
    vector<T>* vektor;
    VectorSize size;
};

template<typename T>
VectorWithSize<T> operator%(vector<T>& vektor, VectorSize size) {
    return VectorWithSize<T>{&vektor, size};
}

struct MatrixSize {
    int rows;
    int columns;
};

class VariableNamesExtractor {
private:
    queue<string> names_;

public:
    VariableNamesExtractor(string names) {
        for (string name : extractVariableNames(names)) {
            names_.push(name);
        }
    }

    string nextName() {
        string name = names_.front();
        names_.pop();
        return name;
    }

private:
    static vector<string> extractVariableNames(const string& s) {
        vector<string> names;
        for (string namePossiblyWithSize : StringUtils::split(s, ',')) {
            vector<string> tokens = StringUtils::splitAndTrimBySpace(namePossiblyWithSize);
            names.push_back(tokens[0]);
        }
        return names;
    }
};

class MagicLineIOSegmentBuilder {
private:
    LineIOSegmentBuilder* builder_;
    VariableNamesExtractor extractor_;

public:
    MagicLineIOSegmentBuilder(LineIOSegmentBuilder& builder, string names)
            : builder_(&builder)
            , extractor_(VariableNamesExtractor(names))
    {}

    template<typename T, typename = ScalarCompatible<T>>
    MagicLineIOSegmentBuilder& operator,(T& var) {
        builder_->addScalarVariable(Scalar::create(var, extractor_.nextName()));
        return *this;
    }

    template<typename T, typename = ScalarCompatible<T>>
    MagicLineIOSegmentBuilder& operator,(vector<T>& var) {
        builder_->addVectorVariable(Vector::create(var, extractor_.nextName()));
        return *this;
    }

    template<typename T, typename = ScalarCompatible<T>>
    MagicLineIOSegmentBuilder& operator,(VectorWithSize<T> var) {
        builder_->addVectorVariable(Vector::create(*var.vektor, extractor_.nextName()), var.size.size);
        return *this;
    }
};

class MagicLinesIOSegmentBuilder {
private:
    LinesIOSegmentBuilder* builder_;
    VariableNamesExtractor extractor_;

public:
    MagicLinesIOSegmentBuilder(LinesIOSegmentBuilder& builder, string names)
            : builder_(&builder)
            , extractor_(VariableNamesExtractor(names))
    {}

    template<typename T, typename = ScalarCompatible<T>>
    MagicLinesIOSegmentBuilder& operator,(vector<T>& var) {
        builder_->addVectorVariable(Vector::create(var, extractor_.nextName()));
        return *this;
    }

    template<typename T, typename = ScalarCompatible<T>>
    MagicLinesIOSegmentBuilder& operator,(vector<vector<T>>& var) {
        builder_->addJaggedVectorVariable(Matrix::create(var, extractor_.nextName()));
        return *this;
    }

    MagicLinesIOSegmentBuilder& operator%(VectorSize size) {
        builder_->setSize(size.size);
        return *this;
    }
};

class MagicGridIOSegmentBuilder {
private:
    GridIOSegmentBuilder* builder_;
    VariableNamesExtractor extractor_;

public:
    MagicGridIOSegmentBuilder(GridIOSegmentBuilder& builder, string names)
            : builder_(&builder)
            , extractor_(VariableNamesExtractor(names))
    {}

    template<typename T, typename = ScalarCompatible<T>>
    MagicGridIOSegmentBuilder& operator,(vector<vector<T>>& var) {
        builder_->addMatrixVariable(Matrix::create(var, extractor_.nextName()));
        return *this;
    }

    MagicGridIOSegmentBuilder& operator%(MatrixSize size) {
        builder_->setSize(size.rows, size.columns);
        return *this;
    }
};

}
