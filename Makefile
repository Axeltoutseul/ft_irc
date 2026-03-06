NAME		= ircserv
SRCS		= Channel.cpp \
			Client.cpp \
			main.cpp \
			parsing.cpp \
			Server.cpp \
				
				
LIBS		= 

SRC_DIR		= .
LIB_DIR		= ./lib
BUILD_DIR	= ./build

CC			= c++
CFLAGS		= -g -Wall -Werror -Wextra -MMD -MP -std=c++98
INCLUDE_OBJ	= ${addprefix -I,${LIBS_DIR}}
INCLUDE_SRC	= ${addprefix -L,${LIBS_DIR}}

SRC			= ${addprefix ${BUILD_DIR}/,${SRCS}}
LIBS_DIR	= ${addprefix ${LIB_DIR}/,${LIBS}}
OBJ			= ${SRC:.cpp=.o}
DPS			= ${SRC:.o=.d}
UNAME_S		= $(shell uname -s)

all: ${NAME}

${NAME}: ${BUILD_DIR} ${OBJ}
	$(CC) $(CFLAGS) ${OBJ} ${INCLUDE_SRC} -o ${NAME}

${BUILD_DIR}:
	mkdir -p ${BUILD_DIR}

${BUILD_DIR}/%.o:${SRC_DIR}/%.cpp
	$(CC) $(CFLAGS) ${INCLUDE_OBJ} -c -o $@ $^

archive: ${BUILD_DIR}
	ar -rc ${NAME}.a ${OBJ}

clean:
	rm -rf ${BUILD_DIR}

fclean: clean
	rm -f ${NAME}
	rm -f ${NAME}.a

re: fclean all

-include $(DPS)

.PHONY: clean fclean re archive