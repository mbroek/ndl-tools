# Top-level makefile for ndl-tools package
# $Id$

include Makefile.global

OTHER		= ChangeLog COPYING FILE_ID.DIZ.in INSTALL.in Makefile \
		  Makefile.global.in config.h.in aclocal.m4 configure \
		  configure.in
TARFILE		= ${PACKAGE}-${VERSION}.tar.bz2

##########################################################################

all depend:
		@for d in ${SUBDIRS}; do (cd $$d && ${MAKE} -w $@) || exit; done;

install:
		@for d in ${SUBDIRS}; do (cd $$d && ${MAKE} -w $@) || exit; done;


dist tar:	${TARFILE}

clean:
		rm -f .filelist core ${TARFILE} config.h
		for d in ${SUBDIRS}; do (cd $$d && ${MAKE} $@) || exit; done;

${TARFILE}:	.filelist
		cd ..; ln -s ${PACKAGE} ${PACKAGE}-${VERSION} ; rm -f ${TARFILE}; \
		${TAR} cvTf ./${PACKAGE}-${VERSION}/.filelist - | bzip2 >${TARFILE} ; \
		rm -f ${PACKAGE}-${VERSION}

.filelist filelist:
		(for f in ${OTHER} ;do echo ${PACKAGE}-${VERSION}/$$f; done) >.filelist
		for d in ${SUBDIRS}; do (cd $$d && ${MAKE} filelist && cat filelist >>../.filelist) || exit; done;


