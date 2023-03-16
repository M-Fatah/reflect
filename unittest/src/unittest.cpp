#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

int
main(int argc, char **argv)
{
	doctest::Context context;

	context.applyCommandLine(argc, argv);

	// Don't break in the debugger when assertions fail.
	context.setOption("no-breaks", true);

	int res = context.run();
	if (context.shouldExit())
		return res;

	return res;
}