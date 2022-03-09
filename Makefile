default: run

%: %.cpp
	clang++ -std=c++11 -O3 -g $< -o $@

run: cube
	./$<

render: cube
	./$< -v | ./render.py
