package huffman;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;

/** Class to demonstrate Huffman coding
 * 
 * @author ciaraldi
 *
 */

public class Huffman {

	String plaintextFileName; // Name of the plaintext file.
	String codedtextFileName; // Name of the encoded file.
	int plaintextFileSize = 0;
	
	char[] fileContents;
	int[] freq = new int[256];
	int[] rank = new int[256];
	String[] codes = new String[256];
	byte[] encodedOutput;

	final static int INBUF_SIZE = 1024; // Number of bytes in the input buffer
	
	// You will need to define a class to hold a tree node,
	// and a variable in this class to hold the root of the tree.

	/** Instantiates a Huffman object
	 * 
	 */
	public Huffman() {
	}

	/** Reads in a plaintext file which you can later Huffman encode.
	 *  Note: This is treated as a binary file.
	 *  
	 * @param filename Name of the file to read in.
	 * @return Number of bytes read.
	 * @exception FileNotFoundException, IOException
	 */
	public int readPlaintext(String fileName) throws FileNotFoundException, IOException {

		plaintextFileName = fileName; // Remember file name
		byte[] plaintext = new byte[0];// this holds the entire plaintext
		
		// First, try to open the file. If it fails, will throw FileNotFoundException.
		FileInputStream inputStream = new FileInputStream(plaintextFileName);
		byte buffer[] = new byte[INBUF_SIZE];
		
		int read = 0;
		
		while((read = inputStream.read(buffer)) != -1){ //read the file while there is more input
			byte[] old = plaintext; //save the old array
			plaintext = new byte[old.length + read]; //re-allocate the array with the new total size
			System.arraycopy(old, 0, plaintext, 0, old.length); //copy the old array to the new one
			System.arraycopy(buffer, 0, plaintext, old.length, read); // copy the buffer to new array
		}
		
        inputStream.close(); // close the file
        
        fileContents = new String(plaintext).toCharArray(); //convert input to a string
        
        plaintextFileSize = fileContents.length;
        return plaintext.length;
	}
	
	/** Analyzes the plaintext file and produces the tree. 
	 * @exception Exception
	 */
	public void analyze() throws Exception {
		if (plaintextFileSize == 0) {
			throw new Exception("File not read yet.");
		}
		
		for(int i = fileContents.length-1; i >= 0; i--){
			freq[fileContents[i]]++;
		}
		
		ArrayList<Node> nodes = new ArrayList<Node>();
		for(int i = 0; i < 256; i++){
			if(freq[i] != 0){
				nodes.add(new Node((char)i, freq[i]));
			}
		}
		Collections.sort(nodes);
		while(nodes.size() >= 2){
			
			nodes.add(new Node(
					nodes.remove(nodes.size()-1),
					nodes.remove(nodes.size()-1)));
			Collections.sort(nodes);
		}
		
		for(int i = 0; i < 256; i++){
			codes[i] = nodes.get(0).getCode((char)i);
		}
		
	}
	
	/** Encodes the plaintext file, using the tree.
	 * 
	 * @exception Exception (not yet implemented)
	 * @return
	 */
	public void encode() {
		StringBuilder encode = new StringBuilder();
		
		for(int i = 0; i < fileContents.length; i++){
			encode.append(codes[fileContents[i]]);
		}
		
		encodedOutput = stringToPackedBytes(encode.toString());	
	}
	
	/**
	 * Converts a binary string to an array of packed bytes
	 * @param s
	 * @return
	 */
	public static byte[] stringToPackedBytes(String s) {
		int bSize = (((s.length() - 1) >> 3) + 1);
		byte[] b = new byte[bSize];

		while (s.length() < bSize * 8) {
			s += "0";
		}

		for (int i = 0; i < bSize; i++) {
			for (int k = i * 8; k < i * 8 + 8; k++) {
				b[i] = (byte) ((b[i] + (s.charAt(k) - '0')) << 1);
			}
			b[i] = (byte) (b[i] >> 1);
		}
		return b;
	}
	
	/** Outputs the encoded string to a file, with 
	 * a name the same as the plaintext file, but with extension .huf 
	 * 
	 * @exception Something
	 */
	void outputEncoded() {
		String hufFilename = plaintextFileName.substring(0, plaintextFileName.lastIndexOf('.')) + ".huf";	
		try {
			FileOutputStream fos = new FileOutputStream(hufFilename);			
			fos.write(encodedOutput);
			fos.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	/** Outputs the frequency information to a file, with 
	 * a name the same as the plaintext file, but with extension .frq 
	 * 
	 * @exception Something
	 */
	void outputFrequencies() {
		String frqFilename = plaintextFileName.substring(0, plaintextFileName.lastIndexOf('.')) + ".frq";	
		try {
			FileOutputStream fos = new FileOutputStream(frqFilename);
			byte[] freqBytes = new byte[freq.length * 4];
			for(int i = 0; i < freq.length; i++){
				freqBytes[i*4+0] = (byte)(freq[i] >> 24);
				freqBytes[i*4+1] = (byte)(freq[i] >> 16);
				freqBytes[i*4+2] = (byte)(freq[i] >>  8);
				freqBytes[i*4+3] = (byte)(freq[i]);
			}
			fos.write(freqBytes);
			fos.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	
}

