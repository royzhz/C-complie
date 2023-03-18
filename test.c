//int c(int a,int b){
//    b=a;
//    int a=0;//t1
//    {
//        int a=1;//t3
//        a=2;//t3
//    }
//    a=2;//t1
//    return a;
//}
//
//int main(){
//    int a=1;
//    int b=2;
//    int c=3;
//    int d=4;
//
//    while(a<c and b<d){
//        if(a==1){
//            c=c+1;
//        }
//        else{
//            while(a<=d){
//                a=a+2;
//            }
//        }
//    }
//    d=c(a,b);
//}

int program(int a,int b,int c)
{
    int i;
    int j;
    i=0;
    if(a>(b+c))
    {
        j=a+(b*c+1);
    }
    else
    {
        j=a;
    }
    while(i<=100)
    {
        i=j*2;
    }
    return i;
}
int demo(int a)
{
    a=a+2;
    return a*2;
}
int main()
{
    int a;
    int b;
    int c;
    a=3;
    b=4;
    c=2;
    a=a-b;
    a=program(a+b,b,demo(c)) ;

}
