package Solution;

import java.io.IOException;
import java.util.Scanner;

public class vigCipher {

	char[][] table;
	
	char[] alphabets = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
		public vigCipher()
		{
			
		}
		
		
		
		//   this code creates the required table
		public void  alphaTable()
		 {
			 table = new char[26][26];
				
				
				
				for(int i=0;i< 26;i++)
				{
					for (int j=0 ;j< 26; j++)
					{
						int val = i+j;
						if(val>25)
						{
							val = val - 26;
						}
						table[i][j] = alphabets[val];
					}
				}
				
				System.out.println("The Alphabet table is:");
				System.out.println();
				
				for(int i=0;i< 26;i++)
				{
					for (int j=0 ;j< 26; j++)
					{
						System.out.print(table[i][j]+" ");
					}
					System.out.println();
				}
				System.out.println();
		 }
		
		// this function creates the requred table 
		
		
		// funtion for encryption 
		
		public String encrypt(char[] plainText , char[] key) 
		{
			
			int plen = plainText.length;
			System.out.println("the length of plain text "+plen);
			int klen = key.length;
			System.out.println("the length of key "+klen);
			char[] temp = new char[plen];
			
			char[] result = new char[plen];
			
			String finalResult;
			
			for(int i =0 ; i<plen ; i++ )
			{
				int val = i;
				if(val != 0)
				{
					val = i%klen;
				}
				temp[i] = key[val];
			}
			
			
			for(int j = 0 ; j < plen ; j++)
			{
				int row ; 
				int col;
				row = plainText[j] - 97;
				col = temp[j] - 97;
				
				result[j] = table[row][col];
				
			}
			
			System.out.println("----------------------------------The plain text :");

			
			for(int i =0 ; i<plen ; i++ )
			{
				
				System.out.print(plainText[i]+" ");
			}
			System.out.println();
			System.out.println("---------------------------------------The repeated key is:");
			for(int i =0 ; i<plen ; i++ )
			{
				
				System.out.print(temp[i]+" ");
			}
			System.out.println();
			System.out.println("----------------------------------The Final Ciphred Result");
			
			for(int i =0 ; i<plen ; i++ )
			{
				
				System.out.print(result[i]+" ");
			}
			
			finalResult = result.toString();
			
			return finalResult;
			
			
		}
		// funtion for encryption 
		
		
		// function for decryption 
		
		public String decrypt(char[] plainText , char[] key)
		{
			
			int len = plainText.length;
			int klen= key.length;
			char[] temp = new char[len];
			
			
			for(int i =0 ; i<len ; i++ )
			{
				int val = i;
				if(val != 0)
				{
					val = i%klen;
				}
				temp[i] = key[val];
			}
			

				char[] result = new char[len];
				
				String finalResult;
				
				for (int i = 0 ; i<len ; i++ )
				{
					char colElement = plainText[i];
					System.out.print(colElement+" ");
					char rowElement = temp[i];
					System.out.print(rowElement+" ");
					int val = colElement - rowElement;
					if(val < 0)
					{
						val = val+26;
					}
					result[i] = alphabets[val];
				}
				System.out.println();
				

				System.out.println("----------------------------------The plain text :");

				
				for(int i =0 ; i<len ; i++ )
				{
					
					System.out.print(plainText[i]+" ");
				}
				
				System.out.println();
				
				System.out.println("---------------------------------------The repeated key is:");
				for(int i =0 ; i<len ; i++ )
				{
					
					System.out.print(temp[i]+" ");
				}
				System.out.println();
				System.out.println("----------------------------------The Final Deciphred Result");
				
				
				for(int i =0 ; i<len ; i++ )
				{
					
					System.out.print(result[i]+" ");
				}
				
				finalResult = result.toString();
				
				return finalResult;
				
				
				
				
		}
		
		// function for decryption
		

	
	 public static void main(String args[]) throws IOException
	    {
		 
		 vigCipher test = new vigCipher();
		 test.alphaTable();
		 
		 
		 Scanner in  = new Scanner(System.in);
		 
		 while(true)
		 {
			 
			 System.out.println();
			 System.out.println();			 
		 System.out.println("1.Encryption");
		 System.out.println("2.Decryption");
		 
		 int choice = in.nextInt();
		 
		 
		 if(choice == 1)
		 {
			 
			 System.out.println("Enter the Plain Text");
			 String ptext = in.next();
			 System.out.println("Enter the Cipher Key");
			 String ckey = in.next();
			 
			 char[] plaintext = ptext.toCharArray();
			 char[] key= ckey.toCharArray();
			 
			 String result = test.encrypt(plaintext,key);
			 
			// System.out.println(result);
			 
		 }
		 
		 else if(choice == 2)
		 {
			 System.out.println("Enter the Plain Text");
			 String ptext = in.next();
			 System.out.println("Enter the Decipher Key");
			 String dkey = in.next();
			 
			 char[] plaintext = ptext.toCharArray();
			 char[] key= dkey.toCharArray();
			 
			 String result = test.decrypt(plaintext,key);
			 
			// System.out.println(result);
			 
			 
		 }
		 else 
		 {
			 break;
		 }
		 
	    }
		 in.close();
			
	
	    }

}















