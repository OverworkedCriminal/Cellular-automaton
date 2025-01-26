#ifdef CPU_SIMULATOR
#include "application/simulation/test/CpuSimulatorFixture.test.hpp"
#elifdef GPU_SIMULATOR
#include "application/simulation/test/GpuSimulatorFixture.test.hpp"
#endif

TEST_CASE_METHOD(TestFixture, "AIR should not fall down through SAND") {
  set({ 0, 1 }, cell::AIR);
  set({ 0, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall down through WATER_L") {
  set({ 0, 1 }, cell::AIR);
  set({ 0, 0 }, cell::WATER_L);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::WATER_L | cell::WATER_R);
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall down through WATER_R") {
  set({ 0, 1 }, cell::AIR);
  set({ 0, 0 }, cell::WATER_R);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::WATER_R | cell::WATER_L);
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should fall down through AIR") {
  set({ 0, 1 }, cell::SAND);
  set({ 0, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "SAND should fall down through WATER_L") {
  set({ 0, 1 }, cell::SAND);
  set({ 0, 0 }, cell::WATER_L);
  expect({ 0, 1 }, cell::WATER_L);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "SAND should fall down through WATER_R") {
  set({ 0, 1 }, cell::SAND);
  set({ 0, 0 }, cell::WATER_R);
  expect({ 0, 1 }, cell::WATER_R);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "WATER_L should fall down through AIR") {
  set({ 0, 1 }, cell::WATER_L);
  set({ 0, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::WATER_L);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "WATER_R should fall down through AIR") {
  set({ 0, 1 }, cell::WATER_R);
  set({ 0, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 0, 0 }, cell::WATER_R);
  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "WATER_L should not fall down through SAND") {
  set({ 0, 1 }, cell::WATER_L);
  set({ 0, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_L | cell::WATER_R);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should not fall down through SAND") {
  set({ 0, 1 }, cell::WATER_R);
  set({ 0, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_R | cell::WATER_L);
  expect({ 0, 0 }, cell::SAND);
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally left through SAND") {
  set({ 0, 1 }, cell::AIR);
  set({ -1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0 }, cell::SAND);

  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally right through SAND") {
  set({ 0, 1 }, cell::AIR);
  set({ 1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0 }, cell::SAND);

  runTestExpectAllSuccess();
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally left through WATER_L") {
  set({ 0, 1 }, cell::AIR);
  set({ -1, 0, }, cell::WATER_L);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0, }, cell::WATER_L | cell::WATER_R);

  // run twice because moving diagonally happens in one direction at the time
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally left through WATER_R") {
  set({ 0, 1 }, cell::AIR);
  set({ -1, 0, }, cell::WATER_R);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0, }, cell::WATER_R | cell::WATER_L);

  // run twice because moving diagonally happens in one direction at the time
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally right through WATER_L") {
  set({ 0, 1 }, cell::AIR);
  set({ 1, 0, }, cell::WATER_L);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0, }, cell::WATER_L | cell::WATER_R);

  // run test twice because moving diagonally happens in one direction at the time
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "AIR should not fall diagonally right through WATER_R") {
  set({ 0, 1 }, cell::AIR);
  set({ 1, 0, }, cell::WATER_R);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0, }, cell::WATER_R | cell::WATER_L);

  // run test twice because moving diagonally happens in one direction at the time
  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should fall diagonally left through AIR") {
  set({ 0, 1 }, cell::SAND);
  set({ -1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0 }, cell::SAND);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should fall diagonally right through AIR") {
  set({ 0, 1 }, cell::SAND);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0 }, cell::SAND);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not fall diagonally left through WATER_L") {
  set({  0, 1 }, cell::SAND);
  set({ -1, 0 }, cell::WATER_L);
  expect({  0, 1 }, cell::SAND);
  expect({ -1, 0 }, cell::WATER_L | cell::WATER_R);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not fall diagonally left through WATER_R") {
  set({  0, 1 }, cell::SAND);
  set({ -1, 0 }, cell::WATER_R);
  expect({  0, 1 }, cell::SAND);
  expect({ -1, 0 }, cell::WATER_R | cell::WATER_L);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not fall diagonally right through WATER_L") {
  set({ 0, 1 }, cell::SAND);
  set({ 1, 0 }, cell::WATER_L);
  expect({ 0, 1 }, cell::SAND);
  expect({ 1, 0 }, cell::WATER_L | cell::WATER_R);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not fall diagonally right through WATER_R") {
  set({ 0, 1 }, cell::SAND);
  set({ 1, 0 }, cell::WATER_R);
  expect({ 0, 1 }, cell::SAND);
  expect({ 1, 0 }, cell::WATER_R | cell::WATER_L);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should fall diagonally left through AIR") {
  set({ 0, 1 }, cell::WATER_L);
  set({ -1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0 }, cell::WATER_L);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should fall diagonally right through AIR") {
  set({ 0, 1 }, cell::WATER_L);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0 }, cell::WATER_L);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should fall diagonally left through AIR") {
  set({ 0, 1 }, cell::WATER_R);
  set({ -1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ -1, 0 }, cell::WATER_R);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should fall diagonally right through AIR") {
  set({ 0, 1 }, cell::WATER_R);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 1 }, cell::AIR);
  expect({ 1, 0 }, cell::WATER_R);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should not fall diagonally left through SAND") {
  set({ 0, 1 }, cell::WATER_L);
  set({ -1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_L | cell::WATER_R);
  expect({ -1, 0 }, cell::SAND);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should not fall diagonally right through SAND") {
  set({ 0, 1 }, cell::WATER_L);
  set({ 1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_L | cell::WATER_R);
  expect({ 1, 0 }, cell::SAND);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should not fall diagonally left through SAND") {
  set({ 0, 1 }, cell::WATER_R);
  set({ -1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_R | cell::WATER_L);
  expect({ -1, 0 }, cell::SAND);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should not fall diagonally right through SAND") {
  set({ 0, 1 }, cell::WATER_R);
  set({ 1, 0 }, cell::SAND);
  expect({ 0, 1 }, cell::WATER_R | cell::WATER_L);
  expect({ 1, 0 }, cell::SAND);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not move horizontally left through AIR") {
  set({  0, 0 }, cell::SAND);
  set({ -1, 0 }, cell::AIR);
  expect({  0, 0 }, cell::SAND);
  expect({ -1, 0 }, cell::AIR);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "SAND should not move horizontally right through AIR") {
  set({ 0, 0 }, cell::SAND);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 0 }, cell::SAND);
  expect({ 1, 0 }, cell::AIR);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should move horizontally left through AIR") {
  set({  0, 0 }, cell::WATER_L);
  set({ -1, 0 }, cell::AIR);
  expect({  0, 0 }, cell::AIR);
  expect({ -1, 0 }, cell::WATER_L);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L when can't move horizontally left should change to WATER_R") {
  set({ 0, 0 }, cell::WATER_L);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 0 }, cell::WATER_R);
  expect({ 1, 0 }, cell::AIR);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R when can't move horizontally right should change to WATER_L") {
  set({ -1, 0 }, cell::AIR);
  set({  0, 0 }, cell::WATER_R);
  expect({ -1, 0 }, cell::AIR);
  expect({  0, 0 }, cell::WATER_L);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should move horizontally right through AIR") {
  set({ 0, 0 }, cell::WATER_R);
  set({ 1, 0 }, cell::AIR);
  expect({ 0, 0 }, cell::AIR);
  expect({ 1, 0 }, cell::WATER_R);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should not move horizontally left through SAND and should not move right through AIR but should change to WATER_R") {
  set({ -1, 0 }, cell::SAND);
  set({  0, 0 }, cell::WATER_L);
  set({  1, 0 }, cell::AIR);
  expect({ -1, 0 }, cell::SAND);
  expect({  0, 0 }, cell::WATER_R);
  expect({  1, 0 }, cell::AIR);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should not move horizontally right through SAND and should not move left through AIR but should change to WATER_L") {
  set({ -1, 0 }, cell::AIR);
  set({  0, 0 }, cell::WATER_R);
  set({  1, 0 }, cell::SAND);
  expect({ -1, 0 }, cell::AIR);
  expect({  0, 0 }, cell::WATER_L);
  expect({  1, 0 }, cell::SAND);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L and WATER_R race for center position") {
  set({ -1, 0 }, cell::WATER_R);
  set({  0, 0 }, cell::AIR);
  set({  1, 0 }, cell::WATER_L);
  expect({ -1, 0 }, cell::WATER_R);
  expect({  0, 0 }, cell::WATER_L);
  expect({  1, 0 }, cell::AIR);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R and WATER_L race for center position") {
  set({ -1, 0 }, cell::WATER_R);
  set({  0, 0 }, cell::AIR);
  set({  1, 0 }, cell::WATER_L);
  expect({ -1, 0 }, cell::AIR);
  expect({  0, 0 }, cell::WATER_R);
  expect({  1, 0 }, cell::WATER_L);

  runTestExpectAnySuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_L should move horizontally through WATER_R") {
  set({ -1, 0 }, cell::WATER_R);
  set({  0, 0 }, cell::WATER_L);
  expect({ -1, 0 }, cell::WATER_L);
  expect({  0, 0 }, cell::WATER_R);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "WATER_R should move horizontally through WATER_L") {
  set({ 0, 0 }, cell::WATER_R);
  set({ 1, 0 }, cell::WATER_L);
  set({ 0, 0 }, cell::WATER_L);
  set({ 1, 0 }, cell::WATER_R);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "Move down, bottom should have priority") {
  set({ 0,  1 }, cell::SAND);
  set({ 0,  0 }, cell::WATER_L);
  set({ 0, -1 }, cell::AIR);
  expect({ 0,  1 }, cell::SAND);
  expect({ 0,  0 }, cell::AIR);
  expect({ 0, -1 }, cell::WATER_L | cell::WATER_R);

  runTestExpectAllSuccess(2);
}

TEST_CASE_METHOD(TestFixture, "Move down diagonally, bottom should have priority") {
  set({  1,  1 }, cell::SAND);
  set({  0,  0 }, cell::WATER_L);
  set({ -1, -1 }, cell::AIR);
  expect({  1,  1 }, cell::SAND);
  expect({  0,  0 }, cell::AIR);
  expect({ -1, -1 }, cell::WATER_L | cell::WATER_R);

  runTestExpectAnySuccess(2);
}
