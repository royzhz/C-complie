int c(){
    int a=0;//t1
    {
        int a=1;//t3
        a=2;//t3
    }
    a=2;//t1
}

int main(){
    int a;
    int b;
    int c;
    int d;

    while(a<c and b<d){
        if(a==1){
            c=c+1;
        }
        else{
            while(a<=d){
                a=a+1;
            }
        }
    }
}