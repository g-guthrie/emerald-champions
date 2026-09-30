#!/usr/bin/env python3
"""Run source-frozen calibration cases in independent native worker processes."""
from __future__ import annotations
import argparse
from collections import Counter
from concurrent.futures import ThreadPoolExecutor, as_completed
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import time

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(Path(__file__).parent))
import generate_battle_suite as suite


def run_case(case, base, destination):
    started=time.monotonic();name=case['case'];config=(base/case['config']).resolve()
    log=destination/'logs'/(name+'.log');run=destination/'runs'/name
    argv=[sys.executable,str(ROOT/'tools/agent_player/battle_calibration.py'),
          '--config',str(config),'--run-dir',str(run)]
    with log.open('w') as stream:
        try:
            process=subprocess.run(argv,cwd=ROOT,stdout=stream,stderr=subprocess.STDOUT,
                                   env={**os.environ,'PYTHONUNBUFFERED':'1'})
            code=process.returncode;reason=''
        except OSError as error:
            code=-1;reason=str(error)
    path=run/'summary.json'
    summary=json.loads(path.read_text()) if path.exists() else {}
    results=summary.get('results',[])
    return {**case,'exit_code':code,'reason':reason,'elapsed_seconds':round(time.monotonic()-started,2),
            'categories':summary.get('categories',{'invalid':1}),
            'evidence_valid':summary.get('evidence_valid',False),
            'verified_legal_winning_witnesses':summary.get('verified_legal_winning_witnesses',0),
            'seeds':[{key:r.get(key) for key in ('seed','category','reason','decisions','witness_verified')}
                     for r in results],
            'source_fingerprint':summary.get('source_fingerprint'),
            'build_id':summary.get('build_provenance',{}).get('build_id'),
            'log':str(log),'run_dir':str(run)}


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cases',type=Path,required=True)
    parser.add_argument('--run-dir',type=Path,required=True)
    parser.add_argument('--workers',type=int,default=6)
    args=parser.parse_args()
    if not 1<=args.workers<=32:parser.error('workers must be1..32')
    index=args.cases.resolve();cases=json.loads(index.read_text())
    if not isinstance(cases,list) or not cases:parser.error('cases must be a nonempty list')
    names=[case.get('case','') for case in cases]
    if len(set(names))!=len(names) or any(not re.fullmatch(r'[A-Za-z0-9_-]+',name) for name in names):
        parser.error('case names must be unique safe directory names')
    destination=args.run_dir.resolve();destination.mkdir(parents=True,exist_ok=False)
    (destination/'logs').mkdir();(destination/'runs').mkdir()
    fingerprint=suite.source_fingerprint();started=time.monotonic();reports=[]
    (destination/'cases.json').write_text(json.dumps(cases,indent=2)+'\n')
    with ThreadPoolExecutor(max_workers=args.workers) as pool:
        jobs=[pool.submit(run_case,case,index.parent,destination) for case in cases]
        for job in as_completed(jobs):
            result=job.result();reports.append(result)
            totals=Counter()
            for row in reports:totals.update(row['categories'])
            summary={'scope':'Finite audited roster, provisional maximum availability; native measured policy results, no human difficulty grade.',
                     'workers':args.workers,'source_fingerprint':fingerprint,'planned_cases':len(cases),
                     'completed_cases':len(reports),'categories':dict(totals),
                     'elapsed_seconds':round(time.monotonic()-started,2),
                     'results':sorted(reports,key=lambda row:row['case'])}
            (destination/'report.json').write_text(json.dumps(summary,indent=2,sort_keys=True)+'\n')
            print(result['case'],result['categories'],'witnesses',result['verified_legal_winning_witnesses'],
                  'valid',result['evidence_valid'],flush=True)
    summary['source_unchanged']=suite.source_fingerprint()==fingerprint
    (destination/'report.json').write_text(json.dumps(summary,indent=2,sort_keys=True)+'\n')
    return 0 if summary['source_unchanged'] and all(r['evidence_valid'] for r in reports) else 1

if __name__=='__main__':raise SystemExit(main())
