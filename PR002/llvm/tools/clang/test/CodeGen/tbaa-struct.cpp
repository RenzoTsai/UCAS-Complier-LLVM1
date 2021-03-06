// RUN: %clang_cc1 -triple x86_64-apple-darwin -emit-llvm -o - -O1 %s | FileCheck %s
//
// Check that we generate !tbaa.struct metadata for struct copies.
struct A {
  short s;
  int i;
  char c;
  int j;
};

void copy(struct A *a, struct A *b) {
  *a = *b;
}

// CHECK: target datalayout = "{{.*}}p:[[P:64|32]]
// CHECK: call void @llvm.memcpy.p0i8.p0i8.i[[P]](i8* %{{.*}}, i8* %{{.*}}, i[[P]] 16, i32 4, i1 false), !tbaa.struct [[TS:!.*]]

struct B {
  char c1;
  struct A a;
  int ii;
};

void copy2(struct B *a, struct B *b) {
  *a = *b;
}

// CHECK: call void @llvm.memcpy.p0i8.p0i8.i[[P]](i8* %{{.*}}, i8* %{{.*}}, i[[P]] 24, i32 4, i1 false), !tbaa.struct [[TS2:!.*]]

typedef _Complex int T2;
typedef _Complex char T5;
typedef _Complex int T7;
typedef struct T4 { T5 field0; T7 field1; } T4;
typedef union T1 { T2 field0; T4 field1; } T1;

void copy3 (T1 *a, T1 *b) {
  *a = *b;
}

// CHECK: call void @llvm.memcpy.p0i8.p0i8.i[[P]](i8* %{{.*}}, i8* %{{.*}}, i[[P]] 12, i32 4, i1 false), !tbaa.struct [[TS3:!.*]]

// Make sure that zero-length bitfield works.
#define ATTR __attribute__ ((ms_struct))
struct five {
  char a;
  int :0;        /* ignored; prior field is not a bitfield. */
  char b;
  char c;
} ATTR;
void copy4(struct five *a, struct five *b) {
  *a = *b;
}
// CHECK: call void @llvm.memcpy.p0i8.p0i8.i[[P]](i8* %{{.*}}, i8* %{{.*}}, i[[P]] 3, i32 1, i1 false), !tbaa.struct [[TS4:!.*]]

struct six {
  char a;
  int :0;
  char b;
  char c;
};
void copy5(struct six *a, struct six *b) {
  *a = *b;
}
// CHECK: call void @llvm.memcpy.p0i8.p0i8.i[[P]](i8* %{{.*}}, i8* %{{.*}}, i[[P]] 6, i32 1, i1 false), !tbaa.struct [[TS5:!.*]]

// CHECK: [[TS]] = metadata !{i64 0, i64 2, metadata !{{.*}}, i64 4, i64 4, metadata !{{.*}}, i64 8, i64 1, metadata !{{.*}}, i64 12, i64 4, metadata !{{.*}}}
// CHECK: [[CHAR:!.*]] = metadata !{metadata !"omnipotent char", metadata !{{.*}}}
// CHECK: [[INT:!.*]] = metadata !{metadata !"int", metadata [[CHAR]]}
// (offset, size) = (0,1) char; (4,2) short; (8,4) int; (12,1) char; (16,4) int; (20,4) int
// CHECK: [[TS2]] = metadata !{i64 0, i64 1, metadata !{{.*}}, i64 4, i64 2, metadata !{{.*}}, i64 8, i64 4, metadata !{{.*}}, i64 12, i64 1, metadata !{{.*}}, i64 16, i64 4, metadata {{.*}}, i64 20, i64 4, metadata {{.*}}}
// (offset, size) = (0,8) char; (0,2) char; (4,8) char
// CHECK: [[TS3]] = metadata !{i64 0, i64 8, metadata !{{.*}}, i64 0, i64 2, metadata !{{.*}}, i64 4, i64 8, metadata !{{.*}}}
// CHECK: [[TS4]] = metadata !{i64 0, i64 1, metadata [[CHAR]], i64 1, i64 1, metadata [[CHAR]], i64 2, i64 1, metadata [[CHAR]]}
// CHECK: [[TS5]] = metadata !{i64 0, i64 1, metadata [[CHAR]], i64 4, i64 4, metadata [[INT]], i64 4, i64 1, metadata [[CHAR]], i64 5, i64 1, metadata [[CHAR]]}
