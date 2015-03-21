########################################################################
#                                                                      #
# This file is part of openPSTD.                                       #
#                                                                      #
# openPSTD is free software: you can redistribute it and/or modify     #
# it under the terms of the GNU General Public License as published by #
# the Free Software Foundation, either version 3 of the License, or    #
# (at your option) any later version.                                  #
#                                                                      #
# openPSTD is distributed in the hope that it will be useful,          #
# but WITHOUT ANY WARRANTY; without even the implied warranty of       #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        #
# GNU General Public License for more details.                         #
#                                                                      #
# You should have received a copy of the GNU General Public License    #
# along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    #
#                                                                      #
########################################################################

__author__ = 'michiel'

import os
import time
import threading
import numpy as np
import helper

class reader:
    def __init__(self):
        self.simulation_result_path = None
        self.simulation_result_pstd_mesh = None
        self.simulation_scene_desc = None
        self.frames = {}
        self.frame_info_updated = []

    def read_frame(self, domain, frame):
        domain_info = self._get_info_domain_pstd(domain)
        filename = self._get_filename(domain, frame)

        flat_array = np.fromfile(filename, dtype=np.dtype('Float32'))
        data = np.reshape(flat_array, (int(domain_info['Nx']), int(domain_info['Nz'])))
        return {'data': data, 'nx': 50, 'ny': 50, 'scene_desc': self._get_info_domain_scene(domain)}

    def _get_filename(self, domain, frame):
        return os.path.join(self.simulation_result_path, '%s-%06d.bin'%(domain,frame))

    def _get_info_domain_pstd(self, domain):
        for item in self.simulation_result_pstd_mesh['domains']:
            if item['id'] == domain:
                return item

    def _get_info_domain_scene(self, domain):
        for item in self.simulation_scene_desc['domains']:
            if item['id'] == domain:
                return item

    def _data_exits(self, domain, frame):
        return os.path.isfile(self._get_filename(domain, frame))

    def update_frame_info(self):
        frames = {}
        for d in self.get_list_domains():
            frames[d] = []

        files = os.listdir(self.simulation_result_path)
        for f in files:
            path = os.path.join(self.simulation_result_path, f)
            if os.path.isfile(path):
                name, extension = os.path.splitext(f)
                domain, frame = name.split(sep="-")
                frame = int(frame)
                frames[domain].append(frame)

        self.frames = frames
        helper.CallObservers(self.frame_info_updated)

    def get_sequence_frame_numbers(self):
        frame_set = set(self.frames[self.simulation_result_pstd_mesh['domains'][0]['id']])
        for d in self.get_list_domains():
            frame_set &= set(self.frames[d])

        result = list(frame_set)
        result.sort()

        return result

    def get_list_domains(self):
        domains = []
        for d in self.simulation_result_pstd_mesh['domains']:
            domains.append(d['id'])
        return domains

