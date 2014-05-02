package huffman;

import java.io.FileNotFoundException;
import java.io.IOException;

public class HuffmanEncode {

	/** Main function to read a plaintext file and output it as a huffman-encoded file.
	 * 
	 * @param args Name of input file, from the command line.
	 */
	
	public static void main(String args[]) {

		String plaintextFileName = "pg1837.txt"; // Name of the default plaintext file.
		int plaintextSize;
		
		Huffman huff = new Huffman(); // Instantiate the object.
		
		if (args.length > 0) plaintextFileName = args[0];
		
		// try to read in the plaintext file
		try {
			plaintextSize = huff.readPlaintext(plaintextFileName);
		} catch(FileNotFoundException ex) {
			System.err.println("Unable to open file '" + plaintextFileName + "'");
			return; // Error exit!
		} catch(IOException ex) {
			System.err.println("Error reading file '" + plaintextFileName + "'");
			return; // Error exit!
		}
		
		System.out.println("Bytes read from file '" + 
				plaintextFileName + "' = " + plaintextSize);
		
		// Now, build the tree
		try {
			huff.analyze();
		}  catch (Exception e) {
			e.printStackTrace();
			// Print some error message
		}
		
		// Encode the plaintext
		huff.encode();
		
		// And output the result
		huff.outputEncoded();
		huff.outputFrequencies();
		
	}
}
