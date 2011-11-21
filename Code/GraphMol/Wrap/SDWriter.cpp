// $Id$
//
//  Copyright (C) 2003-2010  Rational Discovery LLC
//
//   @@ All Rights Reserved @@
//  This file is part of the RDKit.
//  The contents are covered by the terms of the BSD license
//  which is included in the file license.txt, found at the root
//  of the RDKit source tree.
//

#define NO_IMPORT_ARRAY
#include <boost/python.hpp>
#include <string>

//ours
#include <GraphMol/FileParsers/MolWriters.h>
#include <GraphMol/RDKitBase.h>
#include "rdchem.h"
#include <RDBoost/PySequenceHolder.h>
#include <RDBoost/python_streambuf.h>

namespace python = boost::python;
using boost_adaptbx::python::streambuf;

namespace RDKit {
  SDWriter *getSDWriter(python::object &fileobj){
    // FIX: minor leak here
    streambuf *sb=new streambuf(fileobj);
    streambuf::ostream *ost=new streambuf::ostream(*sb);
    return new SDWriter(ost,true);
  }
  
  void SetSDWriterProps(SDWriter &writer, python::object props) {
    // convert the python list to a STR_VECT
    STR_VECT propNames;
    PySequenceHolder<std::string> seq(props);
    for (unsigned int i = 0; i < seq.size(); i++) {
      propNames.push_back(seq[i]);
    }
    writer.setProps(propNames);
  }
  void WriteMolToSD(SDWriter &writer, ROMol &mol, int confId) {
    writer.write(mol, confId);
  }

  struct sdwriter_wrap {
    static void wrap() {
      std::string docStr="Constructor.\n\n"
        "   If a string argument is provided, it will be treated as the name of the output file.\n"
        "   If a file-like object is provided, output will be sent there.\n\n";
      python::class_<SDWriter,
        boost::noncopyable>("SDWriter",
                            "A class for writing molecules to SD files.\n",
                            python::no_init)
        .def("__init__", python::make_constructor(&getSDWriter))
        .def(python::init<std::string>(python::args("fileName"),
                                       docStr.c_str()))
	.def("SetProps", SetSDWriterProps,
	     "Sets the properties to be written to the output file\n\n"
	     "  ARGUMENTS:\n\n"
	     "    - props: a list or tuple of property names\n\n")
	.def("write", WriteMolToSD,
             (python::arg("self"), python::arg("mol"), python::arg("confId")=-1),
	     "Writes a molecule to the output file.\n\n"
	     "  ARGUMENTS:\n\n"
	     "    - mol: the Mol to be written\n"
             "    - confId: (optional) ID of the conformation to write\n\n")
	.def("flush", &SDWriter::flush,
	     "Flushes the output file (forces the disk file to be updated).\n\n"
	     )
	.def("close", &SDWriter::close,
	     "Flushes the output file and closes it. The Writer cannot be used after this.\n\n"
	     )
	.def("NumMols", &SDWriter::numMols,
	     "Returns the number of molecules written so far.\n\n"
	     )
	;
    };
  };
}

void wrap_sdwriter() {
  RDKit::sdwriter_wrap::wrap();
}
