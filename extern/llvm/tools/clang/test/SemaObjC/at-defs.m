// RUN: %clang_cc1 -triple i386-unknown-unknown -fobjc-fragile-abi %s -fsyntax-only

@interface Test {
	double a;
}
@end
@implementation Test
@end
@interface TestObject : Test {
@public
  float bar;
  int foo;
}
@end
@implementation TestObject
@end
struct wibble {
  @defs(TestObject)
};


int main(void)
{
	TestObject * a = (id)malloc(100);
	a->foo = 12;
	printf("12: %d\n", ((struct wibble*)a)->foo);
	printf("%d: %d\n", ((char*)&(((struct wibble*)a)->foo)) - (char*)a, ((char*)&(a->foo)) - (char*)a);
	return 0;
}
