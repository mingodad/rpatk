/*
 *  Regular Pattern Analyzer (RPA)
 *  Copyright (c) 2009-2010 Martin Stoilov
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Martin Stoilov <martin@rpasearch.com>
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <time.h>
#include "rmem.h"
#include "rarray.h"
#include "rpadbex.h"
#include "rpagrep.h"
#include "rpagrepdep.h"


int usage(int argc, const char *argv[])
{
	    fprintf(stderr, "RPA Grep with RPA Engine: %s \n", rpa_dbex_version());
		fprintf(stderr, "Copyright (C) 2010 Martin Stoilov\n\n");

		fprintf(stderr, "Usage: \n %s [OPTIONS] <filename>\n", argv[0]);
		fprintf(stderr, " OPTIONS:\n");
		fprintf(stderr, "\t-e patterns         execute pattern\n");
		fprintf(stderr, "\t-f patternfile      read the patterns from a file, the last pattern will be executed\n");
		fprintf(stderr, "\t-c printpatterns    print these patterns when there is a match.\n");
		fprintf(stderr, "\t-C printpatterns    print these patterns at START MATCH END.\n");
		fprintf(stderr, "\t-i                  ignore case.\n");		
		fprintf(stderr, "\t-m                  match from the beginning.\n");		
		fprintf(stderr, "\t-p                  parse the stream.\n");		
		fprintf(stderr, "\t-l                  line mode.\n");
		fprintf(stderr, "\t-16                 force UTF16 encoding.\n");
		fprintf(stderr, "\t-b                  force byte encoding.\n");
		fprintf(stderr, "\t-d                  dump the pattern tree.\n");
		fprintf(stderr, "\t-t                  display time elapsed.\n");
		fprintf(stderr, "\t-L, --list-rules    List all patterns.\n");
		fprintf(stderr, "\t-v                  Display version information.\n");
		fprintf(stderr, "\t-h, --help          Display this help.\n");
		fprintf(stderr, "\t    --dump-info     Display rules info.\n");
		fprintf(stderr, "\t    --dump-records  Display rules parsing records.\n");

		
		return 0;
}


int main(int argc, const char *argv[])
{
	unsigned long sckb = 0;
	int ret, scanned = 0, i;
	rpa_grep_t *pGrep;
	rarray_t *buffers;

	buffers = r_array_create(sizeof(rpa_buffer_t *));
	pGrep = rpa_grep_create();
	if (argc <= 1) {
		usage(argc, argv);
		goto end;
	}

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-t") == 0) {
			pGrep->showtime = 1;
		}
	}

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "/?") == 0 || strcmp(argv[i], "-h") == 0) {
			usage(argc, argv);
			goto end;
		}
	}

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			fprintf(stderr, "RPA Grep with RPA Engine: %s\n", rpa_dbex_version());
			goto end;
		}
	}

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-c") == 0) {
			if (++i < argc) {
				rpa_buffer_t pattern;
				pattern.s = (char*)argv[i];
				pattern.size = strlen(argv[i]);			
				rpa_grep_setup_matched_callback(pGrep, &pattern);
			}
		}
	}

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-C") == 0) {
			if (++i < argc) {
				rpa_buffer_t pattern;
				pattern.s = (char*)argv[i];
				pattern.size = strlen(argv[i]);
				rpa_grep_setup_callback(pGrep, &pattern);
			}
		}
	}


	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-f") == 0) {
			if (++i < argc) {
				rpa_buffer_t *pattern = rpa_buffer_map_file(argv[i]);
				if (pattern) {
					ret = rpa_grep_load_pattern(pGrep, pattern);
					r_array_add(buffers, &pattern);
				} else {
					ret = -1;
				}
				if (ret < 0)
					goto error;
			}
		}
	}

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-e") == 0) {
			if (++i < argc) {
				rpa_buffer_t pattern;
				pattern.s = (char*)argv[i];
				pattern.size = strlen(argv[i]) + 1;
				ret = rpa_grep_load_string_pattern(pGrep, &pattern);
				if (ret < 0)
					goto error;
			}
			
		}
	}

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--dump-code") == 0) {
			if (rpa_dbex_compile(pGrep->hDbex) < 0) {

			}
			rpa_dbex_dumpcode(pGrep->hDbex);
			goto end;
		}
	}


	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--dump-info") == 0) {
			rpa_grep_dump_pattern_info(pGrep);
			goto end;
		}
	}


	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--dump-records") == 0) {
			rpa_grep_dump_pattern_records(pGrep);
			goto end;
		}
	}

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--dump-records") == 0) {
			rpa_grep_dump_pattern_records(pGrep);
			goto end;
		} else if (strcmp(argv[i], "--dump-info") == 0) {
			rpa_grep_dump_pattern_info(pGrep);
			goto end;
		} else if (strcmp(argv[i], "-L") == 0 || strcmp(argv[i], "--list-rules") == 0) {
			rpa_grep_list_patterns(pGrep);
			goto end;
		} else if (strcmp(argv[i], "-d") == 0) {
			if (++i < argc) {
				if (argv[i]) {
					rpa_buffer_t pattern;
					pattern.s = (char*)argv[i];
					pattern.size = strlen(argv[i]);					
					rpa_grep_dump_pattern_tree(pGrep, &pattern);
					goto end;
				}
			}
		} else if (strcmp(argv[i], "-i") == 0) {
			pGrep->icase = 1;
		} else if (strcmp(argv[i], "-l") == 0) {
			pGrep->linemode = 1;
		} else if (strcmp(argv[i], "-m") == 0) {
			pGrep->matchonly = 1;
		} else if (strcmp(argv[i], "-p") == 0) {
			pGrep->matchonly = 2;
		} else if (strcmp(argv[i], "-16") == 0) {
			pGrep->forceEncoding = RPA_GREP_FORCE_UTF16;
		} else if (strcmp(argv[i], "-b") == 0) {
			pGrep->forceEncoding = RPA_GREP_FORCE_BYTE;
		}
		
	}

	if (rpa_dbex_compile(pGrep->hDbex) < 0) {

		goto end;
	}

	/* scan files */
	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			++scanned;
			rpa_grep_scan_path(pGrep, argv[i]);
		} else if (argv[i][1] == 'e' || argv[i][1] == 'f' || argv[i][1] == 'c' || argv[i][1] == 'C'){
			++i;
		}
		
	}

	if (!scanned) {
		rpa_buffer_t *buf = rpa_buffer_loadfile(stdin);
		if (buf) {
			rpa_grep_scan_buffer(pGrep, buf);
			rpa_buffer_destroy(buf);
		}
	}

end:
	for (i = 0; i < r_array_length(buffers); i++) {
		rpa_buffer_destroy(r_array_index(buffers, i, rpa_buffer_t*));
	}
	r_object_destroy((robject_t*)buffers);
	rpa_grep_close(pGrep);
	if (pGrep->showtime) {
		sckb = (unsigned long)(pGrep->scsize/1024);
		unsigned long milsec;
		unsigned long minutes;
		float sec;
		milsec = pGrep->scanmilisec;
		if (milsec == 0)
			milsec = 1;
		minutes = milsec/60000;
		sec = (milsec%60000)/1000.0;
		fprintf(stdout, "\ntime: %0ldm%1.3fs, %ld KB (%ld KB/sec), stack: %ld KB, memory: %ld KB (leaked %ld Bytes)\n",
				minutes, sec, sckb, 1000*sckb/milsec, pGrep->usedstack / 1000, r_debug_get_maxmem()/1000, r_debug_get_allocmem());
	}

	rpa_grep_destroy(pGrep);
	return 0;

error:
	rpa_grep_destroy(pGrep);
	return 1;
}


