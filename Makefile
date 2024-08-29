# qrme is a quantum-resistant encrypted machine learning system designed
# to protect sensitive data and models against potential threats from
# quantum computing. It utilises advanced cryptographic techniques to ensure
# the confidentiality and integrity of the machine learning model and its
# inputs/outputs, even in the face of future quantum attacks.
#
# Copyright (c) 2024 Finbarrs Oketunji
# Written by Finbarrs Oketunji <f@finbarrs.eu>
#
# This file is part of qrme.
#
# qrme is an open-source software: you are free to redistribute
# and/or modify it under the terms specified in version 3 of the GNU
# General Public License, as published by the Free Software Foundation.
#
# qrme is is made available with the hope that it will be beneficial,
# but it comes with NO WARRANTY whatsoever. This includes, but is not limited
# to, any implied warranties of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE. For more detailed information, please refer to the
# GNU General Public License.
#
# You should have received a copy of the GNU General Public License
# along with qrme.  If not, visit <http://www.gnu.org/licenses/>.

# Detect the operating system
UNAME_S := $(shell uname -s)

# Common variables
CC = gcc
CFLAGS = -O3 -I.
LDFLAGS = -loqs -lcrypto -lm

# Source files
SRC = src/encryption.c src/model.c src/utils.c
OBJ = $(SRC:.c=.o)

# Test files
TEST_SRC = tests/test_all.c
TEST_OBJ = $(TEST_SRC:.c=.o)

# OS-specific configurations
ifeq ($(UNAME_S),Darwin)
	# macOS configuration
	LIBOQS_INCLUDE = -I/usr/local/include
	LIBOQS_LIB = -L/usr/local/lib
	# Install liboqs if not already installed
	ifeq ($(shell brew list --formula | grep -q liboqs; echo $$?),1)
		$(shell brew install liboqs)
	endif
else
	# Linux configuration
	LIBOQS_INCLUDE = -I/usr/include
	LIBOQS_LIB = -L/usr/lib
	# Linux package installation
	PACKAGES = gcc libssl-dev liboqs-dev
	$(shell sudo apt-get update && sudo apt-get install -y $(PACKAGES))
endif

all: create_sample_model test_all ## Build all targets

create_sample_model: create_sample_model.c $(OBJ) ## Build the sample model creation tool
	$(CC) $(CFLAGS) $(LIBOQS_INCLUDE) -o $@ $^ $(LIBOQS_LIB) $(LDFLAGS)

test_all: $(TEST_SRC) $(OBJ) ## Build the test runner
	$(CC) $(CFLAGS) $(LIBOQS_INCLUDE) -o $@ $^ $(LIBOQS_LIB) $(LDFLAGS)

%.o: %.c ## Compile object files
	$(CC) $(CFLAGS) $(LIBOQS_INCLUDE) -c $< -o $@

run-sample: create_sample_model ## Run the sample model creation
	./create_sample_model

run-tests: test_all ## Run all tests
	./test_all

clean: ## Clean up build artifacts
	rm -f $(OBJ) $(TEST_OBJ) create_sample_model test_all test_model.bin test_secret.key

help: ## Display help message
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

.PHONY: all run-sample run-tests clean help

.DEFAULT_GOAL := help
