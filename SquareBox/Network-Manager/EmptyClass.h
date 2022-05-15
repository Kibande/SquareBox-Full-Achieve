#pragma once
namespace SquareBox {
	namespace net {
		class EmptyClass
		{
			EmptyClass();
			~EmptyClass();

			/*  a headr only project failed to create a .lib with x64 bit configuration , so this was neccessary  */
			int uselessFunction(int a, int b);
		};

	};
};

