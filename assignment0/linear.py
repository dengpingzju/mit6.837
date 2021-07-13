def trans3f(m,v):
    ret=[]
    for i in range(3):
        d=0
        for j in range(3):
            d+=m[i][j]*v[j]
        ret.append(d)
    return ret
