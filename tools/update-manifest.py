#!/usr/bin/env python3
#
# pip3 install crccheck pyclibrary

import os
import csv
import toml
import pathlib
import sys
import argparse
from pyclibrary import CParser
from crccheck.crc import Crc16Ibm3740

# Most of the events have u32 payloads, this is a list of events that don't
EVENTS_WITHOUT_PAYLOADS = ['TASK_SWITCHED_OUT']

def dir_path(string):
    if os.path.isdir(string):
        return string
    else:
        raise NotADirectoryError(string)

def remove_prefix(text, prefix):
    return text[text.startswith(prefix) and len(prefix):]

def parse_freertos_trace_event_definitions(header_file_path):
    events = []
    parser = CParser([header_file_path])
    for item in parser.defs['values'].items():
        name = item[0]
        event_id = item[1]
        if name != 'TRACE_EVENT_ID_TOP' and name.startswith('TRACE_EVENT_'):
            canonical_name = remove_prefix(name, 'TRACE_EVENT_').upper()
            event_class = canonical_name.split('_')[0].lower()
            desc = 'FreeRTOS {}'.format(canonical_name.replace('_', ' ').lower())
            has_payload = canonical_name not in EVENTS_WITHOUT_PAYLOADS
            is_failure = 'FAILED' in canonical_name
            event = {
                'name': canonical_name,
                'class': event_class,
                'desc': desc,
                'has_payload': has_payload,
                'is_failure': is_failure,
                'id': str(event_id),
            }
            events.append(event)
    return events

def read_csv(path):
    fieldnames = []
    rows = []
    with open(path, 'r', newline='') as f:
        reader = csv.DictReader(f)
        fieldnames = reader.fieldnames
        for r in reader:
            rows.append(r)
    return fieldnames, rows

def write_csv(path, fieldnames, rows):
    with open(path, 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames, lineterminator='\n')
        writer.writeheader()
        writer.writerows(rows)

def hash_task_name_to_probe_id(task_name):
    return Crc16Ibm3740.calc(str.encode(task_name))

parser = argparse.ArgumentParser(description='Updates Modality component manifest files to include FreeRTOS events and probes.')
parser.add_argument('-c', '--component', type=dir_path, required=True, help='Path to component directory')
parser.add_argument('-t', '--task-names', nargs='+', default=[], help='List of task names to add as probes')
args = parser.parse_args()

root_dir = pathlib.Path(__file__).parent.resolve().parent
trace_event_header_file = root_dir.joinpath('source/include/modality_probe_freertos.h')
component_dir = pathlib.Path(args.component)
root_dir.joinpath('examples/m3-qemu/modality-component')
component_file = component_dir.joinpath('Component.toml')
events_file = component_dir.joinpath('events.csv')
probes_file = component_dir.joinpath('probes.csv')

for p in [trace_event_header_file, component_file, events_file, probes_file]:
    if not p.exists():
        sys.exit('Path \'{}\' doesn\'t exist'.format(p))

component = toml.load(component_file)
event_fieldnames, event_rows = read_csv(events_file)
probe_fieldnames, probe_rows = read_csv(probes_file)

trace_events = parse_freertos_trace_event_definitions(trace_event_header_file)

for trace_event in trace_events:
    event_rows = list(filter(lambda i: i['id'] != trace_event['id'], event_rows))

    tags = 'FreeRTOS;' + trace_event['class']
    if trace_event['is_failure']:
        tags = 'FAILURE;{}'.format(tags)

    type_hint = ''
    if trace_event['has_payload']:
        type_hint = 'u32'

    event_rows.append({
        'component_id': component['id'],
        'id': trace_event['id'],
        'name': trace_event['name'],
        'description': trace_event['desc'],
        'tags': tags,
        'type_hint': type_hint,
        'file': '',
        'line': '',
    })

write_csv(events_file, event_fieldnames, event_rows)

for task_name in args.task_names:
    probe_id = str(hash_task_name_to_probe_id(task_name))
    canonical_name = task_name.replace(' ', '_').replace('-', '_').upper()
    probe_rows = list(filter(lambda i: i['id'] != probe_id, probe_rows))
    probe_rows.append({
        'component_id': component['id'],
        'id': probe_id,
        'name': canonical_name,
        'description': "FreeRTOS task '{}'".format(task_name),
        'tags': 'FreeRTOS;task',
        'file': '',
        'line': '',
    })

if len(args.task_names) > 0:
    if probe_fieldnames is None:
        probe_fieldnames = ['component_id', 'id', 'name', 'description', 'tags', 'file', 'line']
    write_csv(probes_file, probe_fieldnames, probe_rows)
