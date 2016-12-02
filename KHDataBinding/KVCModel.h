//
//  KVCModel.h
//
//  Created by gevin.chen on 2015/4/19.
//  Copyright (c) 2015年 gevin.chen. All rights reserved.
//

/*
 
 協助將 json dictionary 轉成指定 class 的 data object
 
 ex:
 透過 api 收到一包 json data
 {
    name:"abc"
    age:16
    wallet:21
    school:"ooxx senier high school"
 }
 
 有個 class 
 @interface UserData : NSObject
 @property (nonatomic) NSString *name;
 @property (nonatomic) NSNumber *age;
 @property (nonatomic) NSNumber *wallet;
 @property (nonatomic) NSString *school;
 @end
 
 // json data 轉成 data model 
 UserData *user = [KVCModel objectWithJSON:jsonData objectClass:[UserData class] keyCorrespond:nil];
 
 */


#import <Foundation/Foundation.h>

@interface KVCModel : NSObject
{
    // model property name 為 key , json dictionary key 為 value,
    NSMutableDictionary *_keyCorrespondDic;
}
 
-(id)initWithDict:(NSDictionary*)dic;

-(void)injectDict:(NSDictionary*)dic;

//  原本預設行為是，會直接找跟 json key 同名的 property 填入資料
//  如果有些 josn 的 key 你想轉成對應不太一樣的 property name，可以用的這個 function 註明
//  例如：
//      address 想填到 object 的一個叫 myAddress 的 property 就可以呼叫
//      [object setProperty:@"myAddress" correspondKey:"address"];
-(void)setProperty:(NSString*)property correspondKey:(NSString*)jsonKey;

-(NSDictionary*)dict;
-(NSString*)jsonString;
-(NSData*)jsonData;




//  把 object 轉成 dictionary，依 property name 轉成相名的 json key
+(NSDictionary*)dictionaryWithObj:(id)object;
+(NSDictionary*)dictionaryWithObj:(id)object keyCorrespond:(NSDictionary*)correspondDic;

//  把 json string 轉成 object
+(id)objectWithJSONString:(NSString*)jsonString objectClass:(Class)cls keyCorrespond:(NSDictionary*)correspondDic;

//  把 json data 轉成 object 或是 array of object
+(id)objectWithJSON:(NSData*)jsonData objectClass:(Class)cls keyCorrespond:(NSDictionary*)correspondDic;

/**
 將某物件轉換成 dictionary，物件的 property 都會變成 dictionary 裡的 key
 
 @param object 待轉換的物件
 @return 轉換完成的 dictionary
 */
+(id)objectWithDictionary:(NSDictionary*)dict objectClass:(Class)cls;


/**
 將某物件轉換成 dictionary，物件的 property 都會變成 dictionary 裡的 key
 
 @param object 待轉換的物件
 @param correspondDic 轉換的 property name， 物件的property name (key)/ 轉到dictionary 時顯示的 key name ( value) <BR>
 例如 MyClass 有個 property 叫 myAge，我轉成 dictionary 時，myAge 想改叫 userAge，這邊我可以傳入
 @{@"myAge":@"userAge"}
 @return 轉換完成的 dictionary
 */
+(id)objectWithDictionary:(NSDictionary*)dict objectClass:(Class)cls keyCorrespond:(NSDictionary*)correspondDic;


/**
 將 dictionary 裡的值，填入到傳入的物件裡，物件的 property 與 dictionary 的 key 同名，值就會填入 property 裡

 @param jsonDic 儲存值的 dictionary
 @param object 欲接收值的物件
 */
+(void)injectDictionary:(NSDictionary*)jsonDic toObject:(id)object;

/**
 將 dictionary 裡的值，填入到傳入的物件裡，物件的 property 與 dictionary 的 key 同名，值就會填入 property 裡
 
 @param jsonDic 儲存值的 dictionary
 @param object 欲接收值的物件
 */
+(void)injectDictionary:(NSDictionary*)jsonDic toObject:(id)object keyCorrespond:(NSDictionary*)correspondDic;

//  把一個物件的值，轉成另一個物件
+(id)objectWithModel:(id)model objectClass:(Class)cls;
//  把一個model的值，填到另一個 model
+(void)injectWithModel:(id)model toObject:(id)object;


//  把 array 的 object 都轉成指定的 class
+(NSMutableArray*)convertArray:(NSArray*)array toClass:(Class)cls keyCorrespond:(NSDictionary*)correspondDic;

//  把 array 的 object 都轉成指定的 dict
+(NSMutableArray*)convertDictionarys:(NSArray*)array keyCorrespond:(NSDictionary*)correspondDic;






@end
