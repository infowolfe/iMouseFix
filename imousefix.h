/*
 *  mousefix.h
 *  iMousefix
 *
 *  Created by Audun Steinholm on 07.08.05.
 *  Copyright 2005 Audun Steinholm. All rights reserved.
 *
 */

// This is just the main function of the
// original mousefix.c, renamed to avoid
// potential namespace problems. I could have
// rewritten the calling interface, but I want to
// keep the changes as insignificant as possible
// in case a new version of the original mousefix
// is released.
int imousefix(int argc, const char** argv);