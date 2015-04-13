# couriersrs - forwarding messages in courier using SRS

This is the preliminary page of couriersrs, an [SRS](http://www.openspf.org/SRS) implementation for the [Courier mail server](http://www.courier-mta.org/).

To build couriersrs you need to have the libpopt and libsrs2 libraries installed as well as a modern C++ compiler.

## How couriersrs is used

Writing a better documentation is on my TODO for couriersrs. But as I just started work on couriersrs, I was not yet able to write a complete documentation. I hope that the following instructions will help to to use couriersrs anyway.

### Prerequisites

Make sure that you have installed the prerequisites for couriersrs on your system. These should be a modern C++ compiler (I am using gcc 4.1.2, but other modern C++ compilers should work as well), the libpopt library for parsing the command line, and the libsrs2 library, that does the actual SRS work.

If you are installing the prerequisites as packages of your Linux distribution, make sure that you do not only install the runtime files of them, but also the development package for the libraries.

On a Debian system the packages you should install are: libsrs2-dev and libpopt-dev.

### Compiling couriersrs

For short: Get the couriersrs package (see below), unpack it, configure it, compile it and install it.

The couriersrs package is unpacked using the tar command. Go to the directory where you downloaded the couriersrs package and type `tar xfvz couriersrs-_version_.tar.gz` followed by the return key. The package will then get unpacked in a subdirectory of the current directory.

Change to the directory where couriersrs has been unpacked to. Now you have to configure the couriersrs package using the configure script. Typically you will use something like `./configure --prefix=/usr --sysconfdir=/etc` for this. This checks that everything couriersrs needs to get build is installed on your system and aranges that courier gets installed as `/usr/bin/couriersrs` and will by default read the SRS secret from the file `/etc/srs_secret`.

If the configure script fails, you should check that all prerequisites are present on your system. If the configure script has finished successfully, it is now time to compile couriersrs. This is done by entering `make` followed by pressing the return key. You will then see how your compiler compiles the couriersrs package.

If compilation of the source has been finished, it is now time to install the couriersrs binary to the location, where it will be used. This is done by entering `make install` again followed by pressing the return key.

To check that couriersrs has been installed, please go to your home directory and run `couriersrs -v`. If installation has succeeded, couriersrs will tell its version and where it checks for the SRS secret by default (i.e. if you do not explicitly provide an SRS secret on couriersrs' command line).

### Configuring forwardings in the courier mailserver using couriersrs

Couriersrs (and SRS in general) needs a secret, that protects the addresses it creates from being missused. Everytime couriersrs creates an address in forward operation or unmaps an address for delivering a bounce mail, it should use the same secret. Else couriersrs will not allow the bounce mail to be routed back to the sender of the original mail.

The easiest way to configure couriersrs to always use the same secret when handling addresses is to place the SRS secret on line one of the text file /etc/srs_secret (the location may be different dependant on the arguments you passed to the configure script above, but you will always see the location couriersrs uses by running `couriersrs -v`). This file that contains the SRS secret must be readable by any user that will call couriersrs on your system. (Which users this might be depends on your courier installation. It may be always the same user which is responsible for delivery of all mail addresses, or it might be your system's users, if courier delivers mail using the user's standard system accounts.) If having the secret in this file is no option for you, you may also pass the secret on the command line of any invocation of couriersrs using the --secret=<secret to use> option.

Now it is time to configure the first forwarding that will use SRS. This is done using .courier files as normal. Without couriersrs you did this by placing the destination e-mail address prefixed by an ampersand (&) symbol. To do an SRS forward, instead of prefixing the destination e-mail address with the ampersand, you will prefix it by a call to the couriersrs program. So for forwarding a message to _user@example.com_ you will place the following line in the .courier file: `|/usr/bin/couriersrs user@example.com`

All other information that couriersrs needs to do the forward will be read from environment variables the courier mail server sets before calling couriersrs. This is the original sender of the message and the original recipient of it. Couriersrs will take the domain of the original recipient's address to build the SRS address. If you do not want to have the created SRS in this domain (e.g. because you do virtual hosting and do want to have all SRS addresses on a common domain), you can pass couriersrs explicitly the domain it uses to build the SRS address using the --srsdomain=<domain> option on couriersrs' command line. Your .courier file might then look like the following: `|/usr/bin/couriersrs --srsdomain=srs.example.org user@example.com`

As we rewrite the envelope source address of an e-mail while forwarding it, a mailserver that cannot deliver the forwarded will not bounce the message back to the original sender, but to the generated SRS address. This is why we also have to configure the mailserver to accept these e-mails, pass it to couriersrs which maps the address back to the original address and forwards it back to there.

All addresses couriersrs generates as SRS addresses start with <q>SRS0-</q> or <q>SRS1-</q>. So you should create the users <q>SRS0</q> and <q>SRS1</q> or <q>srs0</q> and <q>srs1</q> on your mail server (which pair of users you have to create depends on if your courier server operates in case sensitive (use SRS0 and SRS1) or case in-sensitive (use srs0 and srs1) mode. For both users you create a .courier-default file in their home directory containing the following line: `|/usr/bin/couriersrs --reverse`

(Please note that if you are doing virtual hosting, you will need these two users SRS0 and SRS1 on any of your domains, where SRS addresses are generated. Therefore if you do virtual hosting, you might want to use a common domain for creating all SRS addresses, so that you only have to create these two users on this central domain. This is done using the --srsdomain option as described above.)

## Where you can get the source

The source package of couriersrs can be downloaded from [https://couriersrs.com/download](http://couriersrs.com/download/).

* * *

<address>© 2007-2011 Matthias Wimmer — [Couriersrs is a product of Wimmer Informatik](http://wimmer-informatik.eu/) — [Imprint](http://shared.amessage.eu/impressum)</address>
