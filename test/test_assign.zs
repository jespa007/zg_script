function ret_expression_sequence_3(){
	return 1,2,3;
}

function ret_expression_sequence_2(){
	return 1,2;
}


var a=0,b=1,(c,d,e,f=0,1,2,[0,1,2,3]);

/*%a,b,c=0,1,2%
[a,b,c=0,1,2]
(a,b,c=0,1,2)
{a,b,c=0,1,2}
*a,b,c=0,1,2*
!a,b,c=0,1,2!
$a,b,c=0,1,2$
<a,b,c=0,1,2>*/



a,b,c=0,1,2
Console::println("Test left == right --> a:{0} b:{1} c:{2}",a,,b,c);

a,b,c=3,4
Console::println("Test left >  right --> a:{0} b:{1} c:{2}",a,,b,c);

a,b,c=3,4,5,6,7,8,9
Console::println("Test left <  right --> a:{0} b:{1} c:{2}",a,,b,c);

a,b=1,2
Console::println("Test swap <  right --> a:{0} b:{1}",a,b);

a,b=b,a
Console::println("Test swap <  right --> a:{0} b:{1}",a,b);



Console::println("Test return same   --> a:{0} b:{1} c:{2}",ret_expression_sequence_3());

Console::println("Test return less   --> a:{0} b:{1} c:{2}",ret_expression_sequence_2());