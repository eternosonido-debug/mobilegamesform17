import random,collections,json
from pathlib import Path
root=Path(__file__).resolve().parents[1]
r=random.Random(17012026)
def key(b):return tuple(sorted(b))
def solve(b,limit=100000):
 q=collections.deque([(b,[])]); seen={key(b)}
 while q and len(seen)<limit:
  b,path=q.popleft()
  if all(not t or len(t)==4 and len(set(t))==1 for t in b):return path
  empty=False
  for i,t in enumerate(b):
   if not t:continue
   run=1
   while run<len(t) and t[-run-1]==t[-1]:run+=1
   usedempty=False
   for j,u in enumerate(b):
    if i==j or len(u)==4 or u and u[-1]!=t[-1]:continue
    if not u:
     if run==len(t) or usedempty:continue
     usedempty=True
    n=min(run,4-len(u));x=list(b);x[i]=t[:-n];x[j]=u+t[-n:];x=tuple(x);k=key(x)
    if k not in seen:seen.add(k);q.append((x,path+[(i,j)]))
 return None
levels=[]; seen=set()
while len(levels)<24:
 # Reverse individual pieces from solved configuration, guaranteeing color counts.
 b=[list([i]*4) for i in range(1,5)]+[[],[]]
 for _ in range(15+len(levels)*5):
  i,j=r.sample(range(6),2)
  if b[i] and len(b[j])<4 and (len(b[i])==1 or b[i][-1]==b[i][-2]):b[j].append(b[i].pop())
 b=tuple(map(tuple,b));k=key(b)
 if k in seen:continue
 path=solve(b)
 if not path or len(path)<3:continue
 seen.add(k);levels.append((b,path))
levels.sort(key=lambda x:len(x[1]))
# Solutions are recomputed in the C++ test suite.
with open(root/'src/juice_levels.hpp','w') as f:
 f.write('#pragma once\nnamespace fmg {\nstatic const int juiceLevelCount = 24;\n// Bottom-to-top slots. Every board has a verified legal solution.\nstatic const int juiceBoards[24][24] = {\n')
 for b,path in levels:f.write('  {'+','.join(str(v) for t in b for v in list(t)+[0]*(4-len(t)))+'},\n')
 f.write('};\nstatic const int juicePar[24] = {'+','.join(str(len(p)) for b,p in levels)+'};\n} // namespace fmg\n')
print([len(p) for b,p in levels])
