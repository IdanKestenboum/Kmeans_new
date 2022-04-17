def check_two_files_are_similar(file1,file2):
    with open(file1,'r') as f1:
        f1=f1.read().replace('\n',',').split(',')
    with open(file2,'r') as f2:
        f2=f2.read().replace('\n',',').split(',')

    if len(f1)!=len(f2):
        print("sizes are not the same")
    for i in range(len(f1)):
        if f1[i]!=f2[i]:
            print('problem at index ',i,f1[i] ,'does not equal',f2[i])
            
check_two_files_are_similar('output3_test.txt','output_3.txt')
