server:
	g++ main.cpp interface.cpp connection.cpp crypto.cpp log.cpp -o main -lboost_program_options -lcryptopp
	./main -b base.txt -j journal.txt -p 33333
test:
	g++ UnitTest.cpp interface.cpp connection.cpp crypto.cpp log.cpp -o UnitTest -lUnitTest++ -lboost_program_options -lcryptopp
	