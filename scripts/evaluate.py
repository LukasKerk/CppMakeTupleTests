#!/usr/bin/python3

import sys, csv
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator

# Reads the data 
def reading():
    all_data = {}
    csv_reader = csv.DictReader(sys.stdin, delimiter=",")
    for row in csv_reader:
        t = row["type"]
        row.pop("type")
        all_data[t] = row
    return all_data


# imagine the entries in the dic as a matrix.
# After dropping the keys, the matrix is transposed.
def rotateAndInt(dic):
    firstDict = list(dic.items())[0]
    keys = list(firstDict[1].keys())
    res = {ks:[] for ks in keys}
    for _,d in dic.items():
        for k in keys: res[k].append(int(d[k]))
    return res

def format_and_plot(raw_data):
    Names = list(raw_data.keys())
    ind = list(range(len(Names)))
    data = rotateAndInt(raw_data)
    # pyplotlib setup
    fig, ax = plt.subplots()
    fig.subplots_adjust(left=0.1, right=0.8)
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.yaxis.set_major_locator(MaxNLocator(integer=True))
    ax.grid(True, linestyle="--")
    # helper method to make the plotting easier
    B = [0] * len(Names)
    def plot(numbers, color, label, destructor=False):
        nonlocal B
        width = 0.4
        m = width/2
        def add(A,B):    return [a+b for a,b in zip(A,B)]
        def skalar(A,b): return [a+b for a in A]
        if (destructor):
            ax.bar(skalar(ind,+m), numbers, width, color=color, label=label)
        else:
            ax.bar(skalar(ind,-m), numbers, width, bottom=B, color=color, label=label)
            B = add(B,numbers)
    # generating the graphs
    plot(data["Normal"],     'b', "(int)")
    plot(data["Normal2"],    'g', "(int,int)")
    plot(data["Copy"],       'r', "(&)")
    plot(data["Move"],       'm', "(&&)")
    plot(data["CopyAssign"], 'c', "=(&)")
    plot(data["MoveAssign"], 'y', "=(&&)")
    plot(data["Destructor"], 'lightgray', "~()", True)
    # numbers -> names
    num2names = "\n".join([str(i) + ": " + n for i,n in zip(ind, Names)])
    props = dict(boxstyle='round', facecolor='white', alpha=0.5)
    ax.text(1.01,0.5, num2names, transform=ax.transAxes, fontsize=12,
            verticalalignment='top', bbox=props)
    # place legend, add title, create .pdf
    plt.legend(bbox_to_anchor=(1.01, 1),loc='upper left', borderaxespad=0.)
    plt.title("Comparisons for `make_tuple`")
    plt.savefig(fname="visualisation.pdf", bbox_inches="tight")


if __name__ == "__main__":
    raw_data = reading()
    format_and_plot(raw_data)
