// "$Id: file_chooser.h 6233 2008-09-14 07:54:06Z spitzak $"
//
// Copyright 1998-2006 by Bill Spitzak and others.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "fltk-bugs@fltk.org".

#ifndef fltk_file_chooser_h
#define fltk_file_chooser_h

#include "FL_API.h"

namespace fltk {

/// \name fltk/file_chooser.h
//@{

FL_API void use_system_file_chooser(bool = true);

FL_API const char *dir_chooser(const char *message,const char *fname,int relative=0);
FL_API const char *file_chooser(const char *message,const char *pattern,
				const char *filename, int relative = 0);
FL_API void file_chooser_callback(void (*cb)(const char *));

//@}

}

#endif

//
// End of "$Id: file_chooser.h 6233 2008-09-14 07:54:06Z spitzak $".
//
