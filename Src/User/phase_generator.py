for i in range(1, 7, 1):
    for j in range(1, 7, 1): 
        if i != j:
            for k in range(1, 7, 1): 
                if k != j:
                    for l in range(1, 7, 1): 
                        if l != k:
                            for m in range(1, 7, 1): 
                                if m != l:
                                    for n in range(1, 7, 1): 
                                        if n != m:
                                            print(str(i)  + ' ,' + str(j)  + ' ,' + str(k)  + ' ,' + str(l)  + ' ,' + str(m)  + ' ,' + str(n))