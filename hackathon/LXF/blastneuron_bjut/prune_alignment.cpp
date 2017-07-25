/*
 * prune_alignment.cpp
 *
 * Created by He Yishan
 */

#include<iostream>
#include "prune_alignment.h"
using namespace std;

bool export_prune_alignment(QList<NeuronSWC> & lN, QString fileSaveName, QString fileOpenName)
{
    QFile file(fileSaveName);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
        return false;
    QTextStream myfile(&file);
    myfile<<"# generated by Vaa3D Plugin resample_swc"<<endl;
    myfile<<"# source file(s): "<<fileOpenName<<endl;
    myfile<<"# id,type,x,y,z,r,pid"<<endl;
    V3DLONG root_p = -1;
    for (V3DLONG i=0;i<lN.size();i=i+2)
    {
        myfile << i+1 <<" " << lN.at(i).type << " "<< lN.at(i).x <<" "<<lN.at(i).y << " "<< lN.at(i).z << " "<< lN.at(i).r << " " << root_p << "\n"
                  << i+2 <<" " << lN.at(i+1).type << " "<< lN.at(i+1).x <<" "<<lN.at(i+1).y << " "<< lN.at(i+1).z << " "<< lN.at(i+1).r << " " <<i+1 << "\n";
    }
    file.close();
    cout<<"swc file "<<fileSaveName.toStdString()<<" has been generated, size: "<<lN.size()<<endl;
    return true;
}

QList<NeuronSWC> prune_long_alignment(QList<NeuronSWC> &neuron,double thres)
{
    QList<NeuronSWC> result;
    V3DLONG siz = neuron.size();
    double dist;
    for(V3DLONG i=0; i<siz-1;i=i+2)
    {
        dist = sqrt((neuron[i].x-neuron[i+1].x)*(neuron[i].x-neuron[i+1].x)
                +(neuron[i].y-neuron[i+1].y)*(neuron[i].y-neuron[i+1].y)
                +(neuron[i].z-neuron[i+1].z)*(neuron[i].z-neuron[i+1].z));
        if(dist>=0 && dist <=thres){result.push_back(neuron[i]);result.push_back(neuron[i+1]);}
    }
    return result;
}

bool prune_alignment(const V3DPluginArgList & input, V3DPluginArgList & output)
{
    cout<<"----------This is prune_alignment process----------"<<endl;
    vector<char*>* inlist = (vector<char*>*)(input.at(0).p);
    vector<char*>* outlist = NULL;
    vector<char*>* paralist = NULL;
    if(input.size()!=2)
    {
        printf("Please specify one input swc file.\n");
        return false;
    }
    paralist=(vector<char*>*)(input.at(1).p);
    if(paralist->size()!=1)
    {
        printf("Please specify only one parameter- the aligned distance's threshold.\n");
        return false;
    }
    double thres = atof(paralist->at(0));
    QString fileOpenName = QString(inlist->at(0));
    QString fileSaveName;
    if (output.size()==0)
    {
        printf("No outputfile specified.\n");
        fileSaveName = fileOpenName + "_pruned.swc";
    }
    else if (output.size()==1)
    {
        outlist = (vector<char*>*)(output.at(0).p);
        fileSaveName = QString(outlist->at(0));
    }
    else
    {
        printf("You have specified more than 1 output file. \n");
        return false;
    }
    //  prune the long alignment
    NeuronTree nt;
    QList<NeuronSWC>neuron,result;
    if (fileOpenName.toUpper().endsWith(".SWC") || fileOpenName.toUpper().endsWith(".ESWC"))
        nt = readSWC_file(fileOpenName);
    neuron = nt.listNeuron;
    result=prune_long_alignment(neuron,thres);

    if(!export_prune_alignment(result, fileSaveName, fileOpenName))
    {
        printf("fail to write the output swc file.\n");
        return false;
    }

    return true;
}

