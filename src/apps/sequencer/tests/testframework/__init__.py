import os
import sys

module_path = os.path.normpath(
    os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        "../../../../../build/sim/release/src/apps/sequencer/python"
    )
)
sys.path.append(module_path)

from testsim import *

from .controller import Controller
