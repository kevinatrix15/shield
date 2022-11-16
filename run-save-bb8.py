#!/usr/bin/env python3
"""
@file run-save-bb8.py
@brief Script for running the save-bb8 executable and generating plots
of the configuration space and solved path.
@author Kevin Briggs <kevinabriggs@hotmail.com>
@version 1
@date 2022-11-15
"""
import argparse
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
import platform
import subprocess as sp
import sys

###############################################################################
# CONSTANTS
###############################################################################

_PROJECT_DIR = Path(__file__).parent
_BIN_DIR = _PROJECT_DIR / ('win_build/bin/Release' if platform.system() == 'Windows'
  else 'build/bin')

_EXE_NAME = 'save-bb8' + \
    ('.exe' if platform.system() == 'Windows' else '')
SAVE_BB8 = _BIN_DIR / _EXE_NAME

_OUTPUT_DIR = _PROJECT_DIR / 'output'
CONFIG_SPACE_FILE = _OUTPUT_DIR / 'config-space.txt'
SOLUTION_PATH_FILE = _OUTPUT_DIR / 'solution-path.txt'

###############################################################################
# MAIN
###############################################################################

def main():
  # parse command line arguments
  M = sys.argv[1]               # num rows (ny)
  N = sys.argv[2]               # num cols (nx)
  robot_radius = sys.argv[3]    # robot radius (cells)
  obstacle_config = sys.argv[4]

  # run the executable 
  cmd = [str(SAVE_BB8), str(M), str(N), str(robot_radius), str(obstacle_config)]
  try:
    output = sp.check_output(cmd, stderr=sp.STDOUT, universal_newlines=True)
  
  except sp.CalledProcessError as e:
    print('WARNING: running save-bb8 had issues:', '\n',
          'cmd:', e.cmd, '\n',
          'returncode:', e.returncode, '\n',
          'output:', e.output)

  # check for data files generated
  if not CONFIG_SPACE_FILE.is_file():
    raise Exception("Config space file not found: ", CONFIG_SPACE_FILE)
  if not SOLUTION_PATH_FILE.is_file():
    raise Exception("Solution path file found: ", SOLUTION_PATH_FILE)

  # read in the data
  config_space = np.loadtxt(CONFIG_SPACE_FILE, delimiter=' ', skiprows=3)
  solution_path = np.loadtxt(SOLUTION_PATH_FILE, delimiter=' ')

  # generate plot, showing the configuration space with the path mapped onto it
  # NOTE: the path file may be empty if no path is found
  fig, ax = plt.subplots(1)
  p = ax.pcolormesh(config_space, cmap='jet')

  # If no solution is found, the solution_path will be empty. Only plot it if found
  if solution_path.size != 0:
    x = solution_path[:, 0]
    y = solution_path[:, 1]
    plt.plot(x, y, linewidth='5', color='cyan')
    
  plt.show()
  fig.savefig('solution.png')

if __name__ == "__main__":
  main()