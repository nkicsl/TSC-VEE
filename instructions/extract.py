# import sys
#
# d_count = dict()
# d_time = dict()
# i = 0
# with open("./transferFrom.txt", "r") as f:
#     lines = f.readlines()
#     while i < lines.__len__():
#         inst = lines[i].strip('\n')
#         t = int(lines[i + 1].strip('\n'))
#         if(d_count.get(inst) == None):
#             d_count[inst] = 1
#             d_time[inst] = t
#         else:
#             d_count[inst] += 1
#             d_time[inst] += t
#         i += 2
# if sys.argv[1] == '1':
#     for i in d_count:
#         print(i)
# elif sys.argv[1] == '2':
#     for i in d_count:
#         print(d_count[i])
# elif sys.argv[1] == '3':
#     for i in d_count:
#         print(d_time[i])

d_count = dict()
i = 0
with open("./all.txt", "r") as f:
    lines = f.readlines()
    while i < lines.__len__():
        inst = lines[i].strip('\n')
        if(d_count.get(inst) == None):
            d_count[inst] = 1
        else:
            d_count[inst] += 1
        i += 1
for i in d_count:
    print(i)