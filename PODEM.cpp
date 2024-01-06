#include "readckt.h"
void scoap(){
    FILE *fd;
    int max_level =0;
    cp[strlen(cp)-1] = '\0';

    if((fd = fopen(cp,"w")) == NULL){
        printf("File does not exist!\n");
        return;
    }
    lev();

    for (int i = 0; i < Nnodes; i++)
    {        
        Node[i].CC0 = -1;
        Node[i].CC1 = -1;
        Node[i].C0 = -1;        
    }
    for (int i = 0; i < Nnodes; i++)//initialization
    {
        if (Node[i].level == 0)
        {
            Node[i].CC0 =1;
            Node[i].CC1 =1;
        }
        if (Node[i].fout ==0)
        {
            Node[i].C0 = 0;
            if(max_level < Node[i].level)
                max_level = Node[i].level;
        }
    }

    for (int i = 1; i <= max_level; i++)// compute logic,start from lev1
    {
        for (int j = 0; j < Nnodes; j++)
        {
            if (Node[j].level == i)//find which node ->which level
            {
                if (Node[j].type == BRCH)//there is no way we will find PI because PI is in lev0, we start from lev1;
                {
                    Node[j].CC0 =  Node[j].unodes[0]->CC0;
                    Node[j].CC1 =  Node[j].unodes[0]->CC1;
                }

                else if (Node[j].type == XOR)
                {
                            int temp1,temp2,temp3,temp4;
                            temp1 = Node[j].unodes[0]->CC0 + Node[j].unodes[1]->CC0;
                            temp2 = Node[j].unodes[0]->CC1 + Node[j].unodes[1]->CC1;
                            if (temp1<temp2)
                            {
                                Node[j].CC0 =temp1 +1;
                            }
                            else
                            {
                                Node[j].CC0 = temp2 +1;
                            }
                            temp3 = Node[j].unodes[0]->CC1 + Node[j].unodes[1]->CC0;
                            temp4 = Node[j].unodes[1]->CC1 + Node[j].unodes[0]->CC0;
                            if (temp3 <temp4)
                            {
                               Node[j].CC1 =temp3 +1;
                            }
                            else
                            {
                                 Node[j].CC1 =temp4 +1;
                            }
                            
                            
                                                        
                            
                }

                else if (Node[j].type == OR)
                {
                            Node[j].CC0 = Node[j].unodes[0]->CC0;
                            Node[j].CC1 = Node[j].unodes[0]->CC1;
                             for (int k = 1; k < Node[j].fin; k++)
                            {
                                Node[j].CC0 = Node[j].CC0 + (Node[j].unodes[k]->CC0);
                                if (Node[j].unodes[k]->CC1 <Node[j].CC1)
                                {
                                    Node[j].CC1 = Node[j].unodes[k]->CC1;
                                }
                                
                            }
                            Node[j].CC0 =Node[j].CC0 +1;
                            Node[j].CC1 =Node[j].CC1 +1;

                }

                else if (Node[j].type == NOR)
                {
                            int temp1,temp2;
                            temp1 = Node[j].unodes[0]->CC0;
                            temp2 = Node[j].unodes[0]->CC1;
                             for (int k = 1; k < Node[j].fin; k++)
                            {
                                temp1 = temp1 + (Node[j].unodes[k]->CC0);
                                if (Node[j].unodes[k]->CC1 <temp2)
                                {
                                    temp2 = Node[j].unodes[k]->CC0;
                                }
                            }
                            Node[j].CC1 = temp1 +1;
                            Node[j].CC0 = temp2 +1;
                }

                
                else if (Node[j].type == NOT)
                {
                            Node[j].CC0 = Node[j].unodes[0]->CC1 +1;
                            Node[j].CC1 = Node[j].unodes[0]->CC0 +1;

                }

                else if (Node[j].type == NAND)
                {
                            int temp1,temp2;
                            temp1 = Node[j].unodes[0]->CC0;
                            temp2 = Node[j].unodes[0]->CC1;
                             for (int k = 1; k < Node[j].fin; k++)
                            {
                                temp2 = temp2 + (Node[j].unodes[k]->CC1);
                                if (Node[j].unodes[k]->CC0 <temp1)
                                {
                                    temp1 = Node[j].unodes[k]->CC0;
                                }
                            }
                            Node[j].CC1 = temp1 +1;
                            Node[j].CC0 = temp2 +1;
                }
                

                else if (Node[j].type == AND)
                {
                            int temp1,temp2;
                            temp1 = Node[j].unodes[0]->CC0;
                            temp2 = Node[j].unodes[0]->CC1;
                             for (int k = 1; k < Node[j].fin; k++)
                            {
                                temp2 = temp2 + (Node[j].unodes[k]->CC1);
                                if (Node[j].unodes[k]->CC0 <temp1)
                                {
                                    temp1 = Node[j].unodes[k]->CC0;
                                }
                            }
                            Node[j].CC1 = temp2 +1;
                            Node[j].CC0 = temp1 +1;
                }



                else if (Node[j].type == XNOR)
                {
                    int temp1,temp2,temp3,temp4;
                    temp1 = Node[j].unodes[0]->CC0 + Node[j].unodes[1]->CC0;
                    temp2 = Node[j].unodes[0]->CC1 + Node[j].unodes[1]->CC1;
                    temp3 = Node[j].unodes[0]->CC1 + Node[j].unodes[1]->CC0;
                    temp4 = Node[j].unodes[1]->CC1 + Node[j].unodes[0]->CC0;

                    if (temp3<temp4)
                    {
                        Node[j].CC0 = temp3 +1;
                    }
                    else
                    {
                        Node[j].CC0 = temp4 +1;
                    }
                    
                    if (temp1<temp2)
                    {
                       Node[j].CC1 = temp1 +1;
                    }
                    else
                    {
                        Node[j].CC1 = temp2 +1;
                    }                    
                } 

                else if (Node[j].type == BUF)
                {
                    Node[j].CC0 =  Node[j].unodes[0]->CC0 + 1;
                    Node[j].CC1 =  Node[j].unodes[0]->CC1 + 1;
                } 

            }           
        }
    }

    for (int i = max_level; i >=1; i--)// compute logic,start from max_level
    {
        //printf("i = %d\n",i);
        //printf("max_level = %d\n",max_level);
        for (int j = 0; j < Nnodes; j++)
        {
            //printf("j = %d\n",j);
            //printf("gatetype:%d\n",Node[j].type);
            //printf("level now is :%d\n",Node[j].level);

            if (Node[j].level == i)//find which node ->which level
            {
                if (Node[j].type == BRCH)//there is no way we will find PI because PI is in lev0, we start from lev1;
                {
                    //printf("im inside branch\n");
                    int temp,temp1;
                    temp =  Node[j].unodes[0]->indx;
                    temp1 = Node[temp].dnodes[0]-> C0;
                    for (int k = 1; k < Node[temp].fout; k++)
                    {
                        if (Node[temp].dnodes[k]->C0 <temp1)
                        {
                            temp1 =Node[temp].dnodes[k]->C0;
                        }
                        
                    }
                    
                    Node[temp].C0 = temp1;
                }

                else if (Node[j].type == XOR)
                {
                    int temp_x,temp_y,temp_x_CC0,temp_x_CC1,temp_y_CC0,temp_y_CC1;
                    temp_x =  Node[j].unodes[0]->indx;
                    temp_y = Node[j].unodes[1]->indx;
                    temp_x_CC0 = Node[temp_x].CC0;
                    temp_x_CC1 = Node[temp_x].CC0;
                    temp_y_CC0 = Node[temp_y].CC0;
                    temp_y_CC1 = Node[temp_y].CC1;
                    if (temp_x_CC0<temp_x_CC1)
                    {
                        Node[temp_y].C0 = temp_x_CC0 + Node[j].C0 +1;
                    }
                    else
                    {
                        Node[temp_y].C0 = temp_x_CC1 + Node[j].C0 +1;
                    }

                    if (temp_y_CC0<temp_y_CC1)
                    {
                        Node[temp_x].C0 = temp_y_CC0 + Node[j].C0 +1;
                    }
                    else
                    {
                        Node[temp_x].C0 = temp_y_CC1 + Node[j].C0 +1;
                    }                            
                }

                else if (Node[j].type == OR)
                {
                    int temp,temp_store_CC0,temp2;
                    for (int k = 0; k < Node[j].fin; k++)
                    {
                        temp = Node[j].unodes[k]->indx;
                        temp_store_CC0 = 0;
                        for (int z = 0; z < Node[j].fin; z++)
                        {
                            if ((Node[j].unodes[z]->indx) != temp)
                            {
                                temp2 = Node[j].unodes[z]->indx;
                                temp_store_CC0 = temp_store_CC0 + Node[temp2].CC0;
                            }

                        }
                        Node[temp].C0 = temp_store_CC0 +Node[j].C0 +1;
                    }
                }

                else if (Node[j].type == NOR)
                {
                    int temp,temp_store_CC0,temp2;
                    for (int k = 0; k < Node[j].fin; k++)
                    {
                        temp = Node[j].unodes[k]->indx;
                        temp_store_CC0 = 0;
                        for (int z = 0; z < Node[j].fin; z++)
                        {
                            if ((Node[j].unodes[z]->indx) != temp)
                            {
                                temp2 = Node[j].unodes[z]->indx;
                                temp_store_CC0 = temp_store_CC0 + Node[temp2].CC0;
                            }

                        }
                        Node[temp].C0 = temp_store_CC0 +Node[j].C0 +1;
                    }
                }

                
                else if (Node[j].type == NOT)
                {
                    int temp;
                    temp =  Node[j].unodes[0]->indx;
                    Node[temp].C0 = Node[j].C0 +1;

                }

                else if (Node[j].type == NAND)
                {
                    int temp,temp_store_CC1,temp2;
                    for (int k = 0; k < Node[j].fin; k++)
                    {
                        temp = Node[j].unodes[k]->indx;
                        temp_store_CC1 = 0;
                        for (int z = 0; z < Node[j].fin; z++)
                        {
                            if ((Node[j].unodes[z]->indx) != temp)
                            {
                                temp2 = Node[j].unodes[z]->indx;
                                temp_store_CC1 = temp_store_CC1 + Node[temp2].CC1;
                            }

                        }
                        Node[temp].C0 = temp_store_CC1 +Node[j].C0 +1;
                    }
                }
                

                else if (Node[j].type == AND)
                {
                    int temp,temp_store_CC1,temp2;
                    for (int k = 0; k < Node[j].fin; k++)
                    {
                        temp = Node[j].unodes[k]->indx;
                        temp_store_CC1 = 0;
                        for (int z = 0; z < Node[j].fin; z++)
                        {
                            if ((Node[j].unodes[z]->indx) != temp)
                            {
                                temp2 = Node[j].unodes[z]->indx;
                                temp_store_CC1 = temp_store_CC1 + Node[temp2].CC1;
                            }

                        }
                        Node[temp].C0 = temp_store_CC1 +Node[j].C0 +1;
                    }
                }



                else if (Node[j].type == XNOR)
                {
                    int temp_x,temp_y,temp_x_CC0,temp_x_CC1,temp_y_CC0,temp_y_CC1;
                    temp_x =  Node[j].unodes[0]->indx;
                    temp_y = Node[j].unodes[1]->indx;
                    temp_x_CC0 = Node[temp_x].CC0;
                    temp_x_CC1 = Node[temp_x].CC0;
                    temp_y_CC0 = Node[temp_y].CC0;
                    temp_y_CC1 = Node[temp_y].CC1;
                    if (temp_x_CC0<temp_x_CC1)
                    {
                        Node[temp_y].C0 = temp_x_CC0 + Node[j].C0 +1;
                    }
                    else
                    {
                        Node[temp_y].C0 = temp_x_CC1 + Node[j].C0 +1;
                    }

                    if (temp_y_CC0<temp_y_CC1)
                    {
                        Node[temp_x].C0 = temp_y_CC0 + Node[j].C0 +1;
                    }
                    else
                    {
                        Node[temp_x].C0 = temp_y_CC1 + Node[j].C0 +1;
                    }                     
                } 

                else if (Node[j].type == BUF)
                {
                    int temp;
                    temp =  Node[j].unodes[0]->indx;
                    Node[temp].C0 = Node[j].C0 +1;
                } 

            }
        }           
    }

   

for (int i = 0; i < Nnodes; i++)
{
    //fprintf(fd,"node:%d,CC0:%d,CC1:%d,C0:%d\n",Node[i].num,Node[i].CC0,Node[i].CC1,Node[i].C0);
    fprintf(fd,"%d,%d,%d,%d\n",Node[i].num,Node[i].CC0,Node[i].CC1,Node[i].C0);
}
fclose(fd);

}

void scoap_simplified(){
    FILE *fd;
    int max_level =0;
    lev();

    for (int i = 0; i < Nnodes; i++)
    {        
        Node[i].CC0 = -1;
        Node[i].CC1 = -1;
        Node[i].C0 = -1;        
    }
    for (int i = 0; i < Nnodes; i++)//initialization
    {
        if (Node[i].level == 0)
        {
            Node[i].CC0 =1;
            Node[i].CC1 =1;
        }
        if (Node[i].fout ==0)
        {
            Node[i].C0 = 0;
            if(max_level < Node[i].level)
                max_level = Node[i].level;
        }
    }

    for (int i = 1; i <= max_level; i++)// compute logic,start from lev1
    {
        for (int j = 0; j < Nnodes; j++)
        {
            if (Node[j].level == i)//find which node ->which level
            {
                if (Node[j].type == BRCH)//there is no way we will find PI because PI is in lev0, we start from lev1;
                {
                    Node[j].CC0 =  Node[j].unodes[0]->CC0;
                    Node[j].CC1 =  Node[j].unodes[0]->CC1;
                }

                else if (Node[j].type == XOR)
                {
                            int temp1,temp2,temp3,temp4;
                            temp1 = Node[j].unodes[0]->CC0 + Node[j].unodes[1]->CC0;
                            temp2 = Node[j].unodes[0]->CC1 + Node[j].unodes[1]->CC1;
                            if (temp1<temp2)
                            {
                                Node[j].CC0 =temp1 +1;
                            }
                            else
                            {
                                Node[j].CC0 = temp2 +1;
                            }
                            temp3 = Node[j].unodes[0]->CC1 + Node[j].unodes[1]->CC0;
                            temp4 = Node[j].unodes[1]->CC1 + Node[j].unodes[0]->CC0;
                            if (temp3 <temp4)
                            {
                               Node[j].CC1 =temp3 +1;
                            }
                            else
                            {
                                 Node[j].CC1 =temp4 +1;
                            }
                            
                            
                                                        
                            
                }

                else if (Node[j].type == OR)
                {
                            Node[j].CC0 = Node[j].unodes[0]->CC0;
                            Node[j].CC1 = Node[j].unodes[0]->CC1;
                             for (int k = 1; k < Node[j].fin; k++)
                            {
                                Node[j].CC0 = Node[j].CC0 + (Node[j].unodes[k]->CC0);
                                if (Node[j].unodes[k]->CC1 <Node[j].CC1)
                                {
                                    Node[j].CC1 = Node[j].unodes[k]->CC1;
                                }
                                
                            }
                            Node[j].CC0 =Node[j].CC0 +1;
                            Node[j].CC1 =Node[j].CC1 +1;

                }

                else if (Node[j].type == NOR)
                {
                            int temp1,temp2;
                            temp1 = Node[j].unodes[0]->CC0;
                            temp2 = Node[j].unodes[0]->CC1;
                             for (int k = 1; k < Node[j].fin; k++)
                            {
                                temp1 = temp1 + (Node[j].unodes[k]->CC0);
                                if (Node[j].unodes[k]->CC1 <temp2)
                                {
                                    temp2 = Node[j].unodes[k]->CC0;
                                }
                            }
                            Node[j].CC1 = temp1 +1;
                            Node[j].CC0 = temp2 +1;
                }

                
                else if (Node[j].type == NOT)
                {
                            Node[j].CC0 = Node[j].unodes[0]->CC1 +1;
                            Node[j].CC1 = Node[j].unodes[0]->CC0 +1;

                }

                else if (Node[j].type == NAND)
                {
                            int temp1,temp2;
                            temp1 = Node[j].unodes[0]->CC0;
                            temp2 = Node[j].unodes[0]->CC1;
                             for (int k = 1; k < Node[j].fin; k++)
                            {
                                temp2 = temp2 + (Node[j].unodes[k]->CC1);
                                if (Node[j].unodes[k]->CC0 <temp1)
                                {
                                    temp1 = Node[j].unodes[k]->CC0;
                                }
                            }
                            Node[j].CC1 = temp1 +1;
                            Node[j].CC0 = temp2 +1;
                }
                

                else if (Node[j].type == AND)
                {
                            int temp1,temp2;
                            temp1 = Node[j].unodes[0]->CC0;
                            temp2 = Node[j].unodes[0]->CC1;
                             for (int k = 1; k < Node[j].fin; k++)
                            {
                                temp2 = temp2 + (Node[j].unodes[k]->CC1);
                                if (Node[j].unodes[k]->CC0 <temp1)
                                {
                                    temp1 = Node[j].unodes[k]->CC0;
                                }
                            }
                            Node[j].CC1 = temp2 +1;
                            Node[j].CC0 = temp1 +1;
                }



                else if (Node[j].type == XNOR)
                {
                    int temp1,temp2,temp3,temp4;
                    temp1 = Node[j].unodes[0]->CC0 + Node[j].unodes[1]->CC0;
                    temp2 = Node[j].unodes[0]->CC1 + Node[j].unodes[1]->CC1;
                    temp3 = Node[j].unodes[0]->CC1 + Node[j].unodes[1]->CC0;
                    temp4 = Node[j].unodes[1]->CC1 + Node[j].unodes[0]->CC0;

                    if (temp3<temp4)
                    {
                        Node[j].CC0 = temp3 +1;
                    }
                    else
                    {
                        Node[j].CC0 = temp4 +1;
                    }
                    
                    if (temp1<temp2)
                    {
                       Node[j].CC1 = temp1 +1;
                    }
                    else
                    {
                        Node[j].CC1 = temp2 +1;
                    }                    
                } 

                else if (Node[j].type == BUF)
                {
                    Node[j].CC0 =  Node[j].unodes[0]->CC0 + 1;
                    Node[j].CC1 =  Node[j].unodes[0]->CC1 + 1;
                } 

            }           
        }
    }

    for (int i = max_level; i >=1; i--)// compute logic,start from max_level
    {
        //printf("i = %d\n",i);
        //printf("max_level = %d\n",max_level);
        for (int j = 0; j < Nnodes; j++)
        {
            //printf("j = %d\n",j);
            //printf("gatetype:%d\n",Node[j].type);
            //printf("level now is :%d\n",Node[j].level);

            if (Node[j].level == i)//find which node ->which level
            {
                if (Node[j].type == BRCH)//there is no way we will find PI because PI is in lev0, we start from lev1;
                {
                    //printf("im inside branch\n");
                    int temp,temp1;
                    temp =  Node[j].unodes[0]->indx;
                    temp1 = Node[temp].dnodes[0]-> C0;
                    for (int k = 1; k < Node[temp].fout; k++)
                    {
                        if (Node[temp].dnodes[k]->C0 <temp1)
                        {
                            temp1 =Node[temp].dnodes[k]->C0;
                        }
                        
                    }
                    
                    Node[temp].C0 = temp1;
                }

                else if (Node[j].type == XOR)
                {
                    int temp_x,temp_y,temp_x_CC0,temp_x_CC1,temp_y_CC0,temp_y_CC1;
                    temp_x =  Node[j].unodes[0]->indx;
                    temp_y = Node[j].unodes[1]->indx;
                    temp_x_CC0 = Node[temp_x].CC0;
                    temp_x_CC1 = Node[temp_x].CC0;
                    temp_y_CC0 = Node[temp_y].CC0;
                    temp_y_CC1 = Node[temp_y].CC1;
                    if (temp_x_CC0<temp_x_CC1)
                    {
                        Node[temp_y].C0 = temp_x_CC0 + Node[j].C0 +1;
                    }
                    else
                    {
                        Node[temp_y].C0 = temp_x_CC1 + Node[j].C0 +1;
                    }

                    if (temp_y_CC0<temp_y_CC1)
                    {
                        Node[temp_x].C0 = temp_y_CC0 + Node[j].C0 +1;
                    }
                    else
                    {
                        Node[temp_x].C0 = temp_y_CC1 + Node[j].C0 +1;
                    }                            
                }

                else if (Node[j].type == OR)
                {
                    int temp,temp_store_CC0,temp2;
                    for (int k = 0; k < Node[j].fin; k++)
                    {
                        temp = Node[j].unodes[k]->indx;
                        temp_store_CC0 = 0;
                        for (int z = 0; z < Node[j].fin; z++)
                        {
                            if ((Node[j].unodes[z]->indx) != temp)
                            {
                                temp2 = Node[j].unodes[z]->indx;
                                temp_store_CC0 = temp_store_CC0 + Node[temp2].CC0;
                            }

                        }
                        Node[temp].C0 = temp_store_CC0 +Node[j].C0 +1;
                    }
                }

                else if (Node[j].type == NOR)
                {
                    int temp,temp_store_CC0,temp2;
                    for (int k = 0; k < Node[j].fin; k++)
                    {
                        temp = Node[j].unodes[k]->indx;
                        temp_store_CC0 = 0;
                        for (int z = 0; z < Node[j].fin; z++)
                        {
                            if ((Node[j].unodes[z]->indx) != temp)
                            {
                                temp2 = Node[j].unodes[z]->indx;
                                temp_store_CC0 = temp_store_CC0 + Node[temp2].CC0;
                            }

                        }
                        Node[temp].C0 = temp_store_CC0 +Node[j].C0 +1;
                    }
                }

                
                else if (Node[j].type == NOT)
                {
                    int temp;
                    temp =  Node[j].unodes[0]->indx;
                    Node[temp].C0 = Node[j].C0 +1;

                }

                else if (Node[j].type == NAND)
                {
                    int temp,temp_store_CC1,temp2;
                    for (int k = 0; k < Node[j].fin; k++)
                    {
                        temp = Node[j].unodes[k]->indx;
                        temp_store_CC1 = 0;
                        for (int z = 0; z < Node[j].fin; z++)
                        {
                            if ((Node[j].unodes[z]->indx) != temp)
                            {
                                temp2 = Node[j].unodes[z]->indx;
                                temp_store_CC1 = temp_store_CC1 + Node[temp2].CC1;
                            }

                        }
                        Node[temp].C0 = temp_store_CC1 +Node[j].C0 +1;
                    }
                }
                

                else if (Node[j].type == AND)
                {
                    int temp,temp_store_CC1,temp2;
                    for (int k = 0; k < Node[j].fin; k++)
                    {
                        temp = Node[j].unodes[k]->indx;
                        temp_store_CC1 = 0;
                        for (int z = 0; z < Node[j].fin; z++)
                        {
                            if ((Node[j].unodes[z]->indx) != temp)
                            {
                                temp2 = Node[j].unodes[z]->indx;
                                temp_store_CC1 = temp_store_CC1 + Node[temp2].CC1;
                            }

                        }
                        Node[temp].C0 = temp_store_CC1 +Node[j].C0 +1;
                    }
                }



                else if (Node[j].type == XNOR)
                {
                    int temp_x,temp_y,temp_x_CC0,temp_x_CC1,temp_y_CC0,temp_y_CC1;
                    temp_x =  Node[j].unodes[0]->indx;
                    temp_y = Node[j].unodes[1]->indx;
                    temp_x_CC0 = Node[temp_x].CC0;
                    temp_x_CC1 = Node[temp_x].CC0;
                    temp_y_CC0 = Node[temp_y].CC0;
                    temp_y_CC1 = Node[temp_y].CC1;
                    if (temp_x_CC0<temp_x_CC1)
                    {
                        Node[temp_y].C0 = temp_x_CC0 + Node[j].C0 +1;
                    }
                    else
                    {
                        Node[temp_y].C0 = temp_x_CC1 + Node[j].C0 +1;
                    }

                    if (temp_y_CC0<temp_y_CC1)
                    {
                        Node[temp_x].C0 = temp_y_CC0 + Node[j].C0 +1;
                    }
                    else
                    {
                        Node[temp_x].C0 = temp_y_CC1 + Node[j].C0 +1;
                    }                     
                } 

                else if (Node[j].type == BUF)
                {
                    int temp;
                    temp =  Node[j].unodes[0]->indx;
                    Node[temp].C0 = Node[j].C0 +1;
                } 

            }
        }           
    }

}

void podem(){
    int node_num;
    int node_indx;
    int fault_value;
    char* output_file;
    FILE* fd;
    bool success = false;
    
    cp[strlen(cp)-1] = '\0';
    node_num = atoi(strtok(cp," "));
    fault_value = atoi(strtok(NULL," "));
    output_file = strtok(NULL," ");    
    scoap_simplified();
    printf("finish scoap\n");
    // FIXME NEED OPEN FILE AND WRITE
    if((fd = fopen(output_file,"w")) == NULL){
        printf("File does not exist!\n");
        return;
    }
    //set all node vals to x
    for(int i = 0; i < Nnodes; i++){
        Node[i].value = 2;
        Node[i].at_fault = 0;
        if(Node[i].num == node_num)
            node_indx = i;
    }
    //printf("im at podem");
    (&Node[node_indx])->at_fault = fault_value+1;
    printf("start to run podem_run\n");
    printf("nodenum:%d, nodeindx:%d\n",Node[node_indx].num, Node[node_indx].indx);
    success = podem_run(node_indx,fault_value); 
    printf("finish podem_run\n");
    //FIXME write PIs to output_file as a generated test pattern.
    if(success)
    {
        for(int i = 0; i < Npi; i++){
            fprintf(fd, "%d",Pinput[i]->num);
            if((i+1) != Npi)
                fprintf(fd,",");
        }
        fprintf(fd,"\n");
        for(int i = 0; i < Npi; i++){
            if(Pinput[i]->value == 2){
                fprintf(fd, "x");
            }
            else if(Pinput[i]->value == 3){
                fprintf(fd, "1");
            }
            else if(Pinput[i]->value == 4){
                fprintf(fd, "0");
            }
            else 
                fprintf(fd, "%d",Pinput[i]->value);
            if((i+1) != Npi)
                fprintf(fd,",");
        }
        fprintf(fd,"\n");
    }
    fclose(fd);
}
/**/
void podem_for_atpg(int node_num,int fault_value,FILE *fd,FILE *fd1,int first_time){
    int node_indx;
    //FILE*fd1;
    bool success = false;    
    // FIXME NEED OPEN FILE AND WRITE
    // if((fd = fopen("podem_atpg.txt","w")) == NULL){
    //     printf("File does not exist!\n");
    //     return;
    // }
    // if((fd1 = fopen(input_file,"w")) == NULL){
    //     printf("File does not exist!\n");
    //     return;
    // }


    //set all node vals to x
    for(int i = 0; i < Nnodes; i++){
        Node[i].value = 2;
        Node[i].at_fault = 0;
        if(Node[i].num == node_num)
            node_indx = i;
    }
    //printf("im at podem");
    (&Node[node_indx])->at_fault = fault_value+1;
    success = podem_run(node_indx,fault_value); 
    
    //FIXME write PIs to output_file as a generated test pattern.
    if(success)
    {
        //printf("I come inside\n\n");
        if(first_time){
            for(int i = 0; i < Npi; i++){
            fprintf(fd, "%d",Pinput[i]->num);
            fprintf(fd1, "%d",Pinput[i]->num);
            if((i+1) != Npi){
               fprintf(fd,",");
               fprintf(fd1,","); 
            }
                
        }
        fprintf(fd,"\n");
        fprintf(fd1,"\n");
        }


        for(int i = 0; i < Npi; i++){
            if(Pinput[i]->value == 2){
                fprintf(fd, "0");
                fprintf(fd1, "0");
            }
            else if(Pinput[i]->value == 3){
                fprintf(fd, "1");
                fprintf(fd1, "1");
            }
            else if(Pinput[i]->value == 4){
                fprintf(fd, "0");
                fprintf(fd1, "0");
            }
            else {
                fprintf(fd, "%d",Pinput[i]->value);
                fprintf(fd1, "%d",Pinput[i]->value);
                
                
            }
            if((i+1) != Npi){
                fprintf(fd,",");
                fprintf(fd1,",");
            }
        }   
        fprintf(fd,"\n");
        fprintf(fd1,"\n");
    }

    

}

// where do we update dfrontier: start of podem_run
// where do we check if fault in activated: at podem_run do a for loop
// where do we update 0 1 to D D': imply -> node_eval
// where do we check if fault in at po: at podem_run do a for loop

void imply(int indx, int val){
    NSTRUC* np = &Node[indx];
    if( np->type != IPT){
        printf("ERROR, IMPLY RECEIVE A NON PI INPUT\n");
    }
    np->value = val;
    node_eval(np);
    list<NSTRUC*> node_list;
    for(int i = 0; i < np->fout; i++){
        //printf("IN FOR LOOP\n");
        node_list.push_back(np->dnodes[i]);
    }
    while(node_list.empty() != 1){
        //printf("IN WHILE LOOP\n");
        np = node_list.front();
        node_eval(np);
        if(np->value != 2){
            // propagate
            for(int i = 0; i < np->fout; i++){
                node_list.push_back(np->dnodes[i]);
            }
        }
        /*
        else{
            for(int i = 0; i < np->fin; i++){
                if(np->unodes[i]->value == 4 ||np->unodes[i]->value == 3){
                    //found d frontier
                    if(d_frontier_set.insert(np->dnodes[i]->indx).second == false){
                        
                    }
                }
            }
        }
        */
        node_list.pop_front();
    }
    // printf("\n\n\n");
    // for (int i = 0; i < Nnodes; i++)
    // {
    //     printf("node:%d, value:%d\n",Node[i].num,Node[i].value);
    // }
    // printf("\n\n\n");
}

// value 3 == D 1/0, value 4 == D' 0/1
bool podem_run(int indx, int ssa){
    // first scan all nodes for d frontier and if fault at PO
    //printf("im inside podem_run\n");
    list<int> d_frontier;
    bool fault_activated = 0;
    //printf("podem run start\n");
    //check if fault is activated
    /*if((&Node[indx])->value == ssa){
        return false;
    }*/
    for (int i = 0; i < Nnodes; i++)
    {
        NSTRUC* np = &Node[i];
        if(np->value == 2){
            if(np->type == BRCH){
                if(np->unodes[0]->value != 2 ){
                    np->value = np->unodes[0]->value;
                    for(int j = 0; j < np->fout; j++){
                        node_eval(np->dnodes[j]);
                    }
                }       
            }
        }
    }
    if(ssa == 0 && (&Node[indx])->value == 3){
        fault_activated = 1;
    }
    else if(ssa == 1 && (&Node[indx])->value == 4){
        fault_activated = 1;
    }
    else if( (&Node[indx])->value != 2 || (&Node[indx])->value == ssa){
        //fault node got assigned to non fault and non x value 
        //which means the propagated value is wrong
        // or node_eval did not update 0 1 to D D'
        // failure due to fault not activatable
        //(&Node[indx])->value = 2;
        return false;
    }
    //printf("podem run start 2\n");
    //check if fault is at po
    for ( int i = 0; i < Npo; i++)
    {
        if(Poutput[i]->value  == 3 || Poutput[i]->value  == 4){
                // D or D' at PO
                return true;
            }
    }
    //update dfrontier
    //printf("podem run start 3 \n");
    if(fault_activated == 1){
        for (int i = Nnodes-1; i >= 0; i--)
        {
            NSTRUC* np = &Node[i];
            int D_flag = 0;
            if(np->value == 2){
                for(int j = 0; j < np->fin; j++){
                    
                    if(np->unodes[j]->value == 3 || np->unodes[j]->value == 4 ){
                        //found fault in input
                        D_flag = 1;
                        if(np->type == BRCH || np->type == BUF || np->type == NOT){
                            printf("BUG IN SEARCHING FOR D FRONTIER: A GATE WITHOUT INPUT CHOICE MIGHT BE ADDED\n");
                        }
                        break;
                    }
                }
                if(D_flag == 1){
                    // add to d frontier
                    d_frontier.push_back(i);
                }
            }
        }
    }
    // check if d frontier is empty 
    // and return fail
    //printf("podem run start 4\n");
    if(d_frontier.size() == 0 &&fault_activated ==1 ){
        return false;
    }
    //printf("podem run start 5\n");
    //printf("obj: indx %d, node num %d, ssa %d, val: %d\n", indx, Node[indx].num, ssa,Node[indx].value);
    //printf("input num %d, input val %d \n", Node[indx].unodes[0]->num,  Node[indx].unodes[0]->value);
    pair<int,int> obj_pair = obj(indx,ssa,fault_activated,d_frontier);
    //printf("obj return: indx %d, node num %d, val %d\n", obj_pair.first, Node[indx].num, obj_pair.second);
    //printf("activated: %d\n", fault_activated);
    //printf("podem run start 6\n");
    
    pair<int,int> PI = backtrace(obj_pair.first,obj_pair.second);
    //printf("line num:%d,value:%d\n",Node[PI.first].num,PI.second);
    //printf("imply start\n");
    imply(PI.first,PI.second);
    //printf("imply finish\n");
    //printf("line num:%d,value after imply:%d\n\n",Node[PI.first].num,Node[PI.first].value);
    if(podem_run(indx,ssa) == true){
        return true;
    }
    imply(PI.first,(PI.second+1)%2);
    if(podem_run(indx,ssa) == true){
        return true;
    }
    imply(PI.first,2);
    return false;
}

pair<int,int> obj(int indx, int val, bool fault_activated, 
    list<int> d_frontier){
    NSTRUC* np = &Node[indx];
    pair<int,int> obj_return;
    int min_c = 1000000000;

    int xor_flag;
    int j,c;
    if(fault_activated == false){
        obj_return.first = indx;
        if( val == 0)
            obj_return.second = 1;
        else
            obj_return.second = 0;        
    }
    else{
        //fault activated, propagate the fault along d frontier to POs
        for(int i = 0; i < d_frontier.size(); i++){
            np = &Node[d_frontier.front()];
            // gate
            if(np->type == OR || np->type == NOR){
                c = 1;
                //np = np->unodes[0];
            }
            else if(np->type == XOR || np->type == XNOR){
                xor_flag = 1;
            }
            else if(np->type == AND|| np->type == NAND){
                c = 0;
            }
            for(int j = 0; j < np->fin; j++){
                // input line
                if(np->unodes[j]->value == 2){
                    // possible choice, take notes
                    int return_val = (c+1)%2;
                    if(return_val == 1){
                        if(np->unodes[j]->CC1 < min_c){
                            min_c = np->unodes[j]->CC1;
                            obj_return.first = np->unodes[j]->indx;
                            obj_return.second = return_val;
                        }
                    }
                    else{
                        if(np->unodes[j]->CC0 < min_c){
                            min_c = np->unodes[j]->CC0;
                            obj_return.first = np->unodes[j]->indx;
                            obj_return.second = return_val;
                        }
                    }
                }
            }
            d_frontier.pop_front();
        }
    }
    return obj_return;
}

pair<int,int> backtrace(int indx, int val){
    //printf("enter backtrace\n");
    NSTRUC* np = &Node[indx];
    int i;
    int v = val;
    int choice0,choice1, min_c1, min_c0;
    int xor_flag;
    //printf("indx: %d, num: %d, node_indx: %d, type: %d \n",indx, np->num, np->indx, np->type);
    //printf("\nnum: %d, val: %d\n", Node[13].num, Node[13].value);
    while(np->type != IPT){
        xor_flag = 0;
        
        // get i,v and next np
        if(np->type == BRCH || np->type == OR || np->type == AND || np->type == BUF){
            i = 0;
            //np = np->unodes[0];
        }
        else if(np->type == XOR || np->type == XNOR){
            xor_flag = 1;
        }
        else if(np->type == NOR || np->type == NOT || np->type == NAND){
            i = 1;
        }

        v = v^i;
        choice0 = -1;
        choice1 = -1;
        min_c1 = 100000000;
        min_c0 = 100000000;
        //int x_flag;
        for(int j = 0; j < np->fin; j++){
            if(np->unodes[j]->value == 2){
                //x_flag = 1;
                if(np->unodes[j]->CC1 < min_c1){
                    min_c1 = np->unodes[j]->CC1;
                    choice1 = np->unodes[j]->indx;
                }
                if(np->unodes[j]->CC0 < min_c0){
                    min_c0 = np->unodes[j]->CC0;
                    choice0 = np->unodes[j]->indx;
                }
            }
        }

        if(xor_flag == 1){
            if(min_c0 < min_c1){
                np = &Node[choice0];
            }
            else{
                np = &Node[choice1];
            }
        }
        else{
            if(v == 1){
                np = &Node[choice1];
            }
            else{
                np = &Node[choice0];
            }
        }
    }
    pair<int,int> PI;
    PI.first = np->indx;
    PI.second = v;
    //printf("backtrace OVER\n\n");
    return PI;
}