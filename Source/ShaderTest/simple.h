namespace {
const char gSimpleFx[] = {\
102, 108, 111, 97, 116, 52, 32, 103, 84, 114, 97, 110, 115, 102, 111, 114, \
109, 91, 32, 52, 32, 93, 32, 58, 32, 114, 101, 103, 105, 115, 116, 101, \
114, 40, 32, 99, 48, 32, 41, 59, 13, 10, 102, 108, 111, 97, 116, 52, \
32, 103, 68, 105, 102, 102, 117, 115, 101, 67, 111, 108, 111, 114, 32, 58, \
32, 114, 101, 103, 105, 115, 116, 101, 114, 40, 32, 99, 49, 48, 32, 41, \
59, 13, 10, 13, 10, 115, 116, 114, 117, 99, 116, 32, 84, 111, 80, 105, \
120, 101, 108, 123, 13, 10, 9, 102, 108, 111, 97, 116, 52, 32, 115, 99, \
114, 101, 101, 110, 80, 111, 115, 105, 116, 105, 111, 110, 32, 58, 32, 80, \
79, 83, 73, 84, 73, 79, 78, 59, 13, 10, 9, 102, 108, 111, 97, 116, \
52, 32, 99, 111, 108, 111, 114, 32, 58, 32, 67, 79, 76, 79, 82, 48, \
59, 32, 47, 47, -23, -96, -126, -25, -126, -71, -29, -126, -85, -29, -125, -87, \
-29, -125, -68, 13, 10, 9, 102, 108, 111, 97, 116, 50, 32, 116, 101, 120, \
67, 111, 111, 114, 100, 32, 58, 32, 84, 69, 88, 67, 79, 79, 82, 68, \
48, 59, 32, 47, 47, -29, -125, -122, -29, -126, -81, -29, -126, -71, -29, -125, \
-127, -29, -125, -93, -27, -70, -89, -26, -88, -103, 13, 10, 125, 59, 13, 10, \
13, 10, 84, 111, 80, 105, 120, 101, 108, 32, 86, 83, 95, 110, 111, 76, \
105, 103, 104, 116, 105, 110, 103, 40, 32, 13, 10, 102, 108, 111, 97, 116, \
52, 32, 97, 80, 111, 115, 105, 116, 105, 111, 110, 32, 58, 32, 80, 79, \
83, 73, 84, 73, 79, 78, 48, 44, 13, 10, 102, 108, 111, 97, 116, 52, \
32, 97, 67, 111, 108, 111, 114, 32, 58, 32, 67, 79, 76, 79, 82, 48, \
44, 13, 10, 102, 108, 111, 97, 116, 50, 32, 97, 84, 101, 120, 67, 111, \
111, 114, 100, 32, 58, 32, 84, 69, 88, 67, 79, 79, 82, 68, 48, 32, \
41, 123, 13, 10, 9, 84, 111, 80, 105, 120, 101, 108, 32, 111, 59, 13, \
10, 9, 111, 46, 115, 99, 114, 101, 101, 110, 80, 111, 115, 105, 116, 105, \
111, 110, 46, 120, 32, 61, 32, 100, 111, 116, 40, 32, 103, 84, 114, 97, \
110, 115, 102, 111, 114, 109, 91, 32, 48, 32, 93, 44, 32, 97, 80, 111, \
115, 105, 116, 105, 111, 110, 32, 41, 59, 13, 10, 9, 111, 46, 115, 99, \
114, 101, 101, 110, 80, 111, 115, 105, 116, 105, 111, 110, 46, 121, 32, 61, \
32, 100, 111, 116, 40, 32, 103, 84, 114, 97, 110, 115, 102, 111, 114, 109, \
91, 32, 49, 32, 93, 44, 32, 97, 80, 111, 115, 105, 116, 105, 111, 110, \
32, 41, 59, 13, 10, 9, 111, 46, 115, 99, 114, 101, 101, 110, 80, 111, \
115, 105, 116, 105, 111, 110, 46, 122, 32, 61, 32, 100, 111, 116, 40, 32, \
103, 84, 114, 97, 110, 115, 102, 111, 114, 109, 91, 32, 50, 32, 93, 44, \
32, 97, 80, 111, 115, 105, 116, 105, 111, 110, 32, 41, 59, 13, 10, 9, \
111, 46, 115, 99, 114, 101, 101, 110, 80, 111, 115, 105, 116, 105, 111, 110, \
46, 119, 32, 61, 32, 100, 111, 116, 40, 32, 103, 84, 114, 97, 110, 115, \
102, 111, 114, 109, 91, 32, 51, 32, 93, 44, 32, 97, 80, 111, 115, 105, \
116, 105, 111, 110, 32, 41, 59, 13, 10, 35, 105, 102, 100, 101, 102, 32, \
76, 73, 78, 69, 65, 82, 95, 90, 66, 85, 70, 70, 69, 82, 13, 10, \
9, 111, 46, 115, 99, 114, 101, 101, 110, 80, 111, 115, 105, 116, 105, 111, \
110, 46, 122, 32, 42, 61, 32, 111, 46, 115, 99, 114, 101, 101, 110, 80, \
111, 115, 105, 116, 105, 111, 110, 46, 119, 59, 13, 10, 35, 101, 110, 100, \
105, 102, 13, 10, 9, 111, 46, 99, 111, 108, 111, 114, 46, 120, 121, 122, \
119, 32, 61, 32, 97, 67, 111, 108, 111, 114, 46, 122, 121, 120, 119, 32, \
42, 32, 103, 68, 105, 102, 102, 117, 115, 101, 67, 111, 108, 111, 114, 32, \
47, 32, 50, 53, 53, 46, 48, 59, 32, 47, 47, 66, 71, 82, 65, -29, \
-127, -89, -26, -99, -91, -29, -126, -117, -29, -127, -82, -29, -127, -89, -23, -96, \
-122, -25, -107, -86, -27, -92, -119, -26, -101, -76, -29, -128, -126, -27, -118, -96, \
-29, -127, -120, -29, -127, -90, 85, 66, 89, 84, 69, 52, -29, -127, -86, -29, \
-127, -82, -29, -127, -89, 50, 53, 53, -29, -127, -89, -27, -119, -78, -29, -127, \
-93, -29, -127, -90, -26, -83, -93, -24, -90, -113, -27, -116, -106, 13, 10, 9, \
111, 46, 116, 101, 120, 67, 111, 111, 114, 100, 32, 61, 32, 97, 84, 101, \
120, 67, 111, 111, 114, 100, 59, 13, 10, 9, 114, 101, 116, 117, 114, 110, \
32, 111, 59, 13, 10, 125, 13, 10, 13, 10, 13, 10, 102, 108, 111, 97, \
116, 52, 32, 66, 97, 115, 105, 99, 95, 80, 83, 40, 32, 84, 111, 80, \
105, 120, 101, 108, 32, 73, 110, 32, 41, 32, 58, 32, 67, 79, 76, 79, \
82, 48, 13, 10, 123, 13, 10, 32, 32, 47, 47, 102, 108, 111, 97, 116, \
52, 32, 99, 111, 108, 111, 114, 59, 13, 10, 32, 32, 47, 47, 99, 111, \
108, 111, 114, 46, 120, 121, 122, 119, 61, 73, 110, 46, 99, 111, 108, 111, \
114, 46, 120, 121, 122, 119, 59, 13, 10, 32, 32, 47, 47, 114, 101, 116, \
117, 114, 110, 32, 99, 111, 108, 111, 114, 59, 13, 10, 32, 32, 114, 101, \
116, 117, 114, 110, 32, 102, 108, 111, 97, 116, 52, 40, 49, 46, 48, 102, \
44, 32, 49, 46, 48, 102, 44, 32, 48, 46, 48, 102, 44, 32, 49, 46, \
48, 102, 32, 41, 59, 13, 10, 13, 10, 125, 13, 10, 116, 101, 99, 104, \
110, 105, 113, 117, 101, 32, 116, 101, 120, 116, 117, 114, 101, 100, 13, 10, \
123, 13, 10, 32, 32, 32, 112, 97, 115, 115, 32, 112, 48, 13, 10, 32, \
32, 32, 123, 13, 10, 32, 32, 32, 32, 32, 32, 86, 101, 114, 116, 101, \
120, 83, 104, 97, 100, 101, 114, 32, 61, 32, 99, 111, 109, 112, 105, 108, \
101, 32, 118, 115, 95, 50, 95, 48, 32, 86, 83, 95, 110, 111, 76, 105, \
103, 104, 116, 105, 110, 103, 40, 41, 59, 13, 10, 32, 32, 32, 32, 32, \
32, 80, 105, 120, 101, 108, 83, 104, 97, 100, 101, 114, 32, 61, 32, 99, \
111, 109, 112, 105, 108, 101, 32, 112, 115, 95, 50, 95, 48, 32, 66, 97, \
115, 105, 99, 95, 80, 83, 40, 41, 32, 59, 13, 10, 32, 32, 32, 125, \
13, 10, 125, 13, 10, 13, 10, \
};
} //anonymous namespace
