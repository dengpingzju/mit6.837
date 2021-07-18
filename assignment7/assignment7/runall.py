import os
exe_name='D:/dengping/course/mit_CG/assignment7/Debug/assignment7.exe'
tasks=[]
while True:
    s=input()
    if s=='':
        continue
    if s=='end':
        break
    if s=='runall':
        cnt=1
        for i in tasks:
            print(str(cnt)+'#: '+i)
            os.system(i)
            cnt+=1
        tasks=[]
        print('finished')
    else:
        s=s[s.find(' ')+1:]
        tasks.append(exe_name+' '+s)
    
    #print(s)