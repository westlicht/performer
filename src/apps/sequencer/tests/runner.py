import os
import unittest

if __name__ == "__main__":
    loader = unittest.TestLoader()
    tests = loader.discover(os.path.dirname(__file__), "*.py")
    # print(tests)
    runner = unittest.runner.TextTestRunner(verbosity=2)
    result = runner.run(tests)
    # print(loader)
    # unittest.main(testLoader=loader)
