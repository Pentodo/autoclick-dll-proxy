dinput8.dll: AutoClickProxy.cpp
	g++ -shared -static-libgcc -static-libstdc++ -static -O2 -o dinput8.dll AutoClickProxy.cpp

clean:
	@echo "Cleaning up build file"
	@rm -f dinput8.dll
