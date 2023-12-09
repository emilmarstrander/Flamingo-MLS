CXX = @c++
include_paths = -I include -I /usr/local/include/openssl/ -I mlspp-main/include -I mlspp-main/lib/bytes/include/ -I mlspp-main/lib/hpke/include/ \
	-I mlspp-main/lib/mls_vectors/include/ -I mlspp-main/lib/tls_syntax/include/
lib_paths = -L /usr/local/lib/ -L /usr/lib/x86_64-linux-gnu/ -L mlspp-main/build/ -L mlspp-main/build/lib/bytes/ -L mlspp-main/build/lib/hpke/ \
	-L mlspp-main/build/lib/mls_vectors/ -L mlspp-main/build/lib/tls_syntax/ -L mlspp-main/ -L mlspp-main/lib/bytes/ -L mlspp-main/lib/hpke/ \
	-L mlspp-main/lib/mls_vectors/ -L mlspp-main/lib/tls_syntax/
libraries= -l mlspp -l bytes -l tls_syntax -l hpke -l ssl -l crypto -l pthread
output_path=./

FLAGS = -std=c++17
NAME = flamingo_mls

all: program
	@mkdir $(output_path)log -p
	@if [ $(output_path) != "./" ]; then\
        cp $(NAME).conf $(output_path)$(NAME).conf;\
    fi

program: network.o general.o cryptography.o mls.o totem.o
	@$(CXX) main.cpp build/network.o build/general.o build/cryptography.o build/mls.o build/totem.o $(include_paths) $(lib_paths) $(libraries) -o $(output_path)$(NAME) $(FLAGS)
	@echo "6/6 program done"

network.o:
	@$(CXX) -c source/network.cpp $(include_paths) -o build/network.o $(FLAGS)
	@echo "1/6 network.o done"

general.o:
	@$(CXX) -c source/general.cpp $(include_paths) -o build/general.o $(FLAGS)
	@echo "2/6 general.o done"

cryptography.o:
	@$(CXX) -c source/cryptography.cpp $(include_paths) $(lib_paths) $(libraries) -o build/cryptography.o $(FLAGS)
	@echo "3/6 cryptography.o done"

mls.o:
	@$(CXX) -c source/mls.cpp $(include_paths) $(lib_paths) $(libraries) -o build/mls.o $(FLAGS)
	@echo "4/6 mls.o done"

totem.o:
	@$(CXX) -c source/totem.cpp $(include_paths) -o build/totem.o $(FLAGS)
	@echo "5/6 totem.o done"