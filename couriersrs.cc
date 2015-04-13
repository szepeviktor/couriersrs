/* ---------------------------------------------------------------------------
 *  couriersrs - Doing SRS forwarding with courier
 *  Copyright (C) 2007  Matthias Wimmer <m@tthias.eu>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * ---------------------------------------------------------------------------
 */

#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include "couriersrs.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <cerrno>
#include <cstring>

namespace couriersrs {
    /**
     * read a message from stdin and pass to sendmail
     *
     * @return 0 on success, 1 on error
     */
    int pass2sendmail(char const* const sendmail, const std::string& prepended_header, char const* const sender, char const* const recipient) {
	// create a pipe for passing the message to sendmail
	int pipe_fd[2];
	if (pipe(pipe_fd)) {
	    std::cout << N_("Error creating pipe: ") << std::strerror(errno) << std::endl;
	    return 1;
	}

	// run sendmail as a child process
	pid_t pid = fork();
	if (pid == -1) {
	    close(pipe_fd[0]);
	    close(pipe_fd[1]);
	    std::cout << N_("Error forking process: ") << std::strerror(errno) << std::endl;
	    return 1;
	}
	if (pid == 0) {
	    // inside child process
	    close(pipe_fd[1]);
	    close(0);
	    dup2(pipe_fd[0], 0);
	    close(pipe_fd[0]);
	    execl(sendmail, sendmail, "-f", sender, recipient, NULL);

	    // there has been an error, if we reach this point
	    return 1;
	}

	// inside parent process
	close(pipe_fd[0]);

	// pass message to sendmail

	// write prepended header
	if (prepended_header != "") {
	    write(pipe_fd[1], prepended_header.c_str(), prepended_header.length());
	}

	// Pass the real message
	while (std::cin) {
	    std::string line;
	    std::getline(std::cin, line);
	    line += "\n";

	    write(pipe_fd[1], line.c_str(), line.length());
	}

	// close the pipe
	close(pipe_fd[1]);
	
	// wait for sendmail to terminate
	int status = 0;
	waitpid(pid, &status, 0);

	// error?
	if (WEXITSTATUS(status)) {
	    std::cout << N_("Error in sendmail coprocess") << std::endl;
	    return 1;
	}

	return 0;
    }
}

int main(int argc, char const** argv) {
    int do_version = 0;
    char const* secret = NULL;
    char const* secret_file = CONFIG_DIR "/srs_secret";
    char const* separator = "-";
    int ret = 0;
    int noforward = 0;
    int noreverse = 0;
    int reverse = 0;
    char const* dest_address = NULL;
    char const* sender = std::getenv("SENDER");
    char const* recipient = std::getenv("RECIPIENT");
    char const* srsdomain = NULL;
    char const* sendmail = "/usr/sbin/sendmail";

    struct poptOption options[] = {
	{ "version", 'v', POPT_ARG_NONE, &do_version, 0, N_("print server version"), NULL},
	{ "secret", 's', POPT_ARG_STRING, &secret, 0, N_("use this as the secret"), "secret"},
	{ "secretfile", 'S', POPT_ARG_STRING, &secret_file, 0, N_("load SRS secret from a file"), "filename"},
	{ "separator", 0, POPT_ARG_STRING, &separator, 0, N_("address separator character"), "separator character"},
	{ "noforward", 0, POPT_ARG_NONE, &noforward, 0, N_("disable SRS forward rewriting"), NULL},
	{ "noreverse", 0, POPT_ARG_NONE, &noreverse, 0, N_("disable SRS reverse rewriting"), NULL},
	{ "reverse", 'r', POPT_ARG_NONE, &reverse, 0, N_("do reverse instead of forward transformation"), NULL},
	{ "address", 0, POPT_ARG_STRING, &sender, 0, N_("overwrite sender address"), "address"},
	{ "alias", 0, POPT_ARG_STRING, &recipient, 0, N_("overwrite recipient address"), "address"},
	{ "srsdomain", 0, POPT_ARG_STRING, &srsdomain, 0, N_("overwrite domain to build SRS addresses"), "domain"},
	{ "sendmail", 0, POPT_ARG_STRING, &sendmail, 0, N_("sendmail executable to use"), "file"},
	POPT_AUTOHELP
	POPT_TABLEEND
    };

    // parse command line options
    poptContext pCtx = poptGetContext(NULL, argc, argv, options, 0);
    while ((ret = poptGetNextOpt(pCtx)) >= 0) {
	switch (ret) {
	    // access argument by poptGetOptArg(pCtx)
	}
    }

    // error parsing command line?
    if (ret < -1) {
	std::cout << poptBadOption(pCtx, POPT_BADOPTION_NOALIAS) << ": " << poptStrerror(ret) << std::endl;
	return 1;
    }

    // get destination address
    if (!reverse && !do_version) {
	dest_address = poptGetArg(pCtx);
	if (!dest_address) {
	    std::cout << N_("No destination address given.") << std::endl;
	    return 1;
	}
    }

    // anything left?
    if (poptPeekArg(pCtx) != NULL) {
	// XXX i20n
	std::cout << N_("Invalid argument: ") << poptGetArg(pCtx) << std::endl;
	return 1;
    }

    // print version information?
    if (do_version) {
	// XXX i20n
	std::cout << PACKAGE << N_(" version ") << VERSION << std::endl << std::endl;
	std::cout << N_("Default location of the SRS secret file is: ") << CONFIG_DIR "/srs_secret" << std::endl;
	return 0;
    }

    // use which address to build the source address for the forwarded message
    if (!srsdomain) {
	srsdomain = recipient;
    }

    // create srs instance
    srs_t* srs = srs_new();

    // set the secret
    if (secret) {
	srs_add_secret(srs, secret);
    } else {
	// we have to read it from the secret file
	std::ifstream secret_file_stream(secret_file, std::ios::in);
	if (!secret_file_stream) {
	    std::cout << N_("Could not open secret file: ") << secret_file << std::endl;
	    return 1;
	}

	char secret[1024] = "";
	secret_file_stream.getline(secret, sizeof(secret));

	if (secret_file_stream.bad()) {
	    std::cout << N_("Could not read secret from file: ") << secret_file << std::endl;
	    return 1;
	}

	srs_add_secret(srs, secret);

	secret_file_stream.close();
    }

    // set the separator character
    if (std::string(separator).length() != 1) {
	std::cout << N_("Address separator must be exactly one character.") << std::endl;
	return 1;
    }
    ret = srs_set_separator(srs, separator[0]);
    if (ret != SRS_SUCCESS) {
	std::cout << N_("Could not set separator character: ") << srs_strerror(ret) << std::endl;
	return 1;
    }

    // noforward?
    ret = srs_set_noforward(srs, noforward);
    if (ret != SRS_SUCCESS) {
	std::cout << N_("Could not set the noforward flag: ") << srs_strerror(ret) << std::endl;
	return 1;
    }

    // noreverse?
    ret = srs_set_noreverse(srs, noreverse);
    if (ret != SRS_SUCCESS) {
	std::cout << N_("Could not set the noreverse flag: ") << srs_strerror(ret) << std::endl;
	return 1;
    }

    // forward or reverse operation?
    if (!reverse) {
	// check addresses are present
	if (!sender || !sender[0]) {
	    std::cout << N_("Sender address could not be determined.") << std::endl;
	    return 1;
	}
	if (!recipient || !recipient[0]) {
	    std::cout << N_("Recipient address could not be determined.") << std::endl;
	    return 1;
	}
	if (!srsdomain || !srsdomain[0]) {
	    std::cout << N_("Domain to build SRS address could not be determined.") << std::endl;
	    return 1;
	}

	// forward operation
	size_t buffer_size = std::string(sender).length()+std::string(srsdomain).length()+65;
	char *buffer = new char[buffer_size];

	// rewrite address
	ret = srs_forward(srs, buffer, buffer_size, sender, srsdomain);
	if (ret != SRS_SUCCESS) {
	    delete buffer;
	    std::cout << N_("Could not rewrite address in forward mode: ") << srs_strerror(ret) << std::endl;
	    return 1;
	}

	// Create 'Delivered-To' header
	std::string delivered_to_header = std::string("Delivered-To: ")+recipient+"\r\n";

	// forward the message by passing it to sendmail
	ret = couriersrs::pass2sendmail(sendmail, delivered_to_header, buffer, dest_address);
	
	// free memory
	delete buffer;

	// we're done
	return ret;
    }

    // reverse operation

    // check that we have the recipient address
    if (!recipient || !recipient[0]) {
	std::cout << N_("Recipient address could not be determined.") << std::endl;
	return 1;
    }

    // unmap the address
    size_t buffer_size = std::string(recipient).length()+1;
    char *buffer = new char[buffer_size];

    // reverse address
    ret = srs_reverse(srs, buffer, buffer_size, recipient);
    if (ret == SRS_ENOTSRSADDRESS || ret == SRS_ENOSRS0HOST || ret == SRS_ENOSRS0USER || ret == SRS_ENOSRS0HASH || ret == SRS_ENOSRS0STAMP ||
	    ret == SRS_ENOSRS1HOST || ret == SRS_ENOSRS1USER || ret == SRS_ENOSRS1HASH || ret == SRS_EBADTIMESTAMPCHAR || ret == SRS_EHASHTOOSHORT ||
	    ret == SRS_ETIMESTAMPOUTOFDATE || ret == SRS_EHASHINVALID) {
	delete buffer;
	std::cout << srs_strerror(ret) << std::endl;
	return 64;
    }
    if (ret != SRS_SUCCESS) {
	delete buffer;
	std::cout << "Error reverting address: " << srs_strerror(ret) << std::endl;
	return 1;
    }

    // Create 'Delivered-To' header
    std::string delivered_to_header = std::string("Delivered-To: ")+recipient+"\r\n";

    // forward the message by passing it to sendmail
    ret = couriersrs::pass2sendmail(sendmail, delivered_to_header, "", buffer);
	
    // free memory
    delete buffer;

    // we're done
    return ret;
}
