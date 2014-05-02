package huffman;

public class Node implements Comparable {
	
	public Character value = null;
	public int freq;
	public Node left = null;
	public Node right = null;
	
	public Node(Character value, int freq) {
		this.value = value;
		this.freq = freq;
	}
	
	public Node(Node left, Node right){
		this.value = null;
		this.freq = left.freq + right.freq;
	}
	
	public String getCode(Character value){
		String code = null;
		
		if(left != null){
			code = left.getCode(value);
			if(code != null){
				code = "0"+code;
			}
		}
		
		if(right != null){
			code = right.getCode(value);
			if(code != null){
				code = "1"+code;
			}
		}		
		
		if(value != null){
			code =  "";
		}
		
		return code;
	}

	@Override
	public int compareTo(Object other) {
		Node n = (Node)other;
		return freq-n.freq;
	}
	
}
