#include "gmock/gmock.h"

#include <sstream>

#include "tcframe/io/GridIOSegmentManipulator.hpp"

using ::testing::Eq;
using ::testing::StrEq;
using ::testing::Test;

using std::istringstream;
using std::ostringstream;

namespace tcframe {

class GridIOSegmentManipulatorTests : public Test {
protected:
    vector<vector<int>> M;
    GridIOSegment* segment = GridIOSegmentBuilder()
            .addMatrixVariable(Matrix::create(M, "M"))
            .setSize(2, 3)
            .build();
};

TEST_F(GridIOSegmentManipulatorTests, Parsing_Successful) {
    istringstream in("1 2 3\n4 5 6\n");

    GridIOSegmentManipulator::parse(segment, &in);
    EXPECT_THAT(M, Eq(vector<vector<int>>{{1, 2, 3}, {4, 5, 6}}));
}

TEST_F(GridIOSegmentManipulatorTests, Printing_Successful) {
    ostringstream out;

    M = {{1, 2, 3}, {4, 5, 6}};

    GridIOSegmentManipulator::print(segment, &out);
    EXPECT_THAT(out.str(), Eq("1 2 3\n4 5 6\n"));
}

TEST_F(GridIOSegmentManipulatorTests, Printing_Failed_RowsMismatch) {
    ostringstream out;

    M = {{1, 2, 3}};

    try {
        GridIOSegmentManipulator::print(segment, &out);
        FAIL();
    } catch (runtime_error& e) {
        EXPECT_THAT(e.what(), StrEq("Number of rows of matrix 'M' unsatisfied. Expected: 2, actual: 1"));
    }
}

TEST_F(GridIOSegmentManipulatorTests, Printing_Failed_ColumnsMismatch) {
    ostringstream out;

    M = {{1, 2, 3}, {4, 5}};

    try {
        GridIOSegmentManipulator::print(segment, &out);
        FAIL();
    } catch (runtime_error& e) {
        EXPECT_THAT(e.what(), StrEq("Number of columns of row 1 of matrix 'M' unsatisfied. Expected: 3, actual: 2"));
    }
}

}
