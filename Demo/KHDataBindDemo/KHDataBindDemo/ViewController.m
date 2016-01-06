//
//  ViewController.m
//  DataBindTest
//
//  Created by GevinChen on 2015/8/30.
//  Copyright (c) 2015年 GevinChen. All rights reserved.
//

#import "ViewController.h"
#import "MyCell.h"
#import "KHDataBinder.h"
#import "APIOperation.h"
#import "UserModel.h"
#import "UserInfoCell.h"
#import "CollectionDemoController.h"
#import <CoreData/CoreData.h>

//#import "AFNetworking.h"
//#import "MyAPISerializer.h"

@interface ViewController () <KHTableViewDelegate>

@property (weak, nonatomic) IBOutlet UITableView *tableView;

@property (weak, nonatomic) IBOutlet UIButton *btnStopRefresh;
@property (weak, nonatomic) IBOutlet UIButton *btnQuery;


@property (nonatomic) NSManagedObjectContext *managedObjectContext;

@end

@implementation ViewController
{
    
    KHTableDataBinder* tableDataBinder;
    
    NSMutableArray<UserModel*> *models;
    NSMutableArray *itemList;
    
    NSOperationQueue *queue;
    
    NSMutableArray *tempArray;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    queue = [[NSOperationQueue alloc] init];
    tempArray = [[NSMutableArray alloc] initWithCapacity:10];
    
    //  init
    tableDataBinder = [[KHTableDataBinder alloc] initWithTableView:self.tableView delegate:self];
    
    //  enable refresh header and footer
    tableDataBinder.refreshHeadEnabled = YES;
    tableDataBinder.refreshFootEnabled = YES;

    //  create bind array
    models = [tableDataBinder createBindArray]; //  section 0
    itemList = [tableDataBinder createBindArray]; // section 1

    
    //  assign event handler
    [tableDataBinder setHeaderTitles: @[@"User Profile",@"Default Cell"]];
    [tableDataBinder addTarget:self
                        action:@selector(btnclick:model:)
                         event:UIControlEventTouchUpInside 
                          cell:[UserInfoCell class] 
                  propertyName:@"btn"];
    [tableDataBinder addTarget:self
                        action:@selector(btnUpdateclick:model:)
                         event:UIControlEventTouchUpInside 
                          cell:[UserInfoCell class] 
                  propertyName:@"btnUpdate"];
    [tableDataBinder addTarget:self
                        action:@selector(valueChanged:model:)
                         event:UIControlEventValueChanged 
                          cell:[UserInfoCell class]
                  propertyName:@"sw"];
    
    [tableDataBinder bindModel:[UserModel class] cell:[UserInfoCell class]];
    tableDataBinder.lastUpdate = [[NSDate date] timeIntervalSince1970];
    [self loadTableView4];
}

-(void)loadTableView4
{
    KHTableCellModel *item1 = [[KHTableCellModel alloc] init];
    item1.text = @"Title1";
    item1.detail = @"detail1";
    KHTableCellModel *item2 = [[KHTableCellModel alloc] init];
    item2.text = @"Title2";
    item2.detail = @"detail2";
    KHTableCellModel *item3 = [[KHTableCellModel alloc] init];
    item3.text = @"Title3";
    item3.detail = @"detail3";
    KHTableCellModel *item4 = [[KHTableCellModel alloc] init];
    item4.text = @"Title4";
    item4.detail = @"detail4";
    
    [itemList addObject:item1];
    [itemList addObject:item2];
    [itemList addObject:item3];
    [itemList addObject:item4];
}


#pragma mark - API

//  第一，我必須要建一個 MovieModel，讓 json 可以轉成一個實體 object
//  第二，我必須要建一個 MovieCell，讓 model 可以把資料填入
//

- (void)userQuery
{
    
    //  http://api.randomuser.me/?results=10

    //  使用自訂的 http connection handle
    //--------------------------------------------------
    NSDictionary* param = @{@"results": @10 };
    APIOperation *api = [[APIOperation alloc] init];
    api.debug = YES;
    [api GET:@"http://api.randomuser.me/" param:param body:nil response:^(APIOperation *api, id responseObject) {
        NSArray *results = responseObject[@"results"];
        NSArray *users = [KVCModel convertArray:results toClass:[UserModel class] keyCorrespond:nil];
        dispatch_async(dispatch_get_main_queue(), ^{
            NSMutableArray *_array = [[NSMutableArray alloc] init];
            for ( int i=0 ; i<users.count ; i++ ) {
                KHCellModel *model = users[i];
                
                //  前半加入 table view，後半先保留起來，用來測試 insert
                if ( i < ( users.count / 2 ) ) {
//                    model.cellHeight = UITableViewAutomaticDimension;
                    [_array addObject:model];
                }
                else{
                    [tempArray addObject:model];
                }
            }
            [models addObjectsFromArray:_array ];
        });
        [tableDataBinder endRefreshing];
    } fail:^(APIOperation *api, NSError *error) {
        NSLog(@"error !");
        [tableDataBinder endRefreshing];
    }];
    [queue addOperation: api ];
    
    //  使用 AFNetworking
    //--------------------------------------------------
//    MyAPISerializer *serializer = [MyAPISerializer new];
//    MyAPIUnSerializer *unserializer = [MyAPIUnSerializer new];
//    AFHTTPRequestOperationManager *manager = [AFHTTPRequestOperationManager manager];
//    manager.requestSerializer = serializer;
//    manager.responseSerializer = unserializer;
//    [manager GET:@"http://api.randomuser.me/?results=10" parameters:nil success:^(AFHTTPRequestOperation *operation, id responseObject) {
//        NSArray *results = responseObject[@"results"];
//        NSArray *users = [KVCModel convertArray:results toClass:[UserModel class]];
//        dispatch_async(dispatch_get_main_queue(), ^{
//            [models addObjectsFromArray: users ];
//        });
//    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
//        NSLog(@"error!");
//    }];
    
    //  使用 AFHTTPRequestOperation
    //--------------------------------------------------
//    MyAPISerializer *serializer = [MyAPISerializer new];
//    NSMutableURLRequest *request = [serializer requestWithMethod:@"POST" URLString:@"http://api.randomuser.me/?results=10" parameters:nil];
//    AFHTTPRequestOperation *operation = [[AFHTTPRequestOperation alloc] initWithRequest:request ];
//    operation.responseSerializer = [MyAPIUnSerializer new];
//    [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation *operation, id responseObject) {
//        NSArray *results = responseObject[@"results"];
//        NSArray *users = [KVCModel convertArray:results toClass:[UserModel class]];
//        dispatch_async(dispatch_get_main_queue(), ^{
//            [models addObjectsFromArray: users ];
//        });
//    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
//        NSLog(@"error!!");
//    }];
//    [queue addOperation: operation ];
    
}



#pragma mark - UI


#pragma mark - Table Bind Event

- (void)tableViewRefreshHead:(nonnull UITableView *)tableView
{
    NSLog(@"refresh");
//    [models removeAllObjects];
//    [self userQuery];
}

- (void)tableViewRefreshFoot:(nonnull UITableView *)tableView
{
    NSLog(@"load more");
//    [self userQuery];
}

-(void)tableView:(UITableView*)tableView didSelectRowAtIndexPath:(nonnull NSIndexPath *)indexPath
{
    NSLog(@"cell click %ld",indexPath.row );
}

#pragma mark - UI Event


- (IBAction)nextClick:(id)sender
{
    CollectionDemoController *vc = [CollectionDemoController new];
    [self presentViewController:vc animated:YES completion:nil];
}

- (void)btnclick:(id)sender model:(KHCellModel*)model
{
    printf("btn click %ld\n", model.index.row );
    [models removeObject:model];
}

- (void)btnUpdateclick:(id)sender model:(KHCellModel*)model
{
    printf("btn update click %ld\n", model.index.row );
    UserModel *umodel = model;
    umodel.testNum = @( [umodel.testNum intValue] + 1 );
    [models update:model];
}


- (void)valueChanged:(id)sender model:(KHCellModel*)model
{
    printf("value changed %ld\n", model.index.row );
}

- (IBAction)searchClick:(id)sender 
{
    [models removeAllObjects];
    [self userQuery ];
}

- (IBAction)addClick:(id)sender 
{
//    [tableDataBinder endRefreshing];
//    [models removeObjectAtIndex:2];
}


- (IBAction)insertClick:(id)sender 
{
    if ( tempArray.count == 0 ) {
        NSLog(@"no temp data");
        return;
    }
    int idx = arc4random() % tempArray.count;
    UserModel *umodel = tempArray[idx];
    [tempArray removeObject:umodel];
    
    int insert_idx = arc4random() % models.count;
    [models insertObject:umodel atIndex: insert_idx ];
    
}


- (IBAction)removeLastClick:(id)sender 
{
    UserModel *umodel = [models lastObject];
    [tempArray insertObject:umodel atIndex:0];
    [models removeLastObject];
}


- (IBAction)replaceClick:(id)sender 
{
    if ( tempArray.count == 0 ) {
        NSLog(@"no temp data");
        return;
    }
    int idx = arc4random() % tempArray.count;
    UserModel *umodel = tempArray[idx];
    [tempArray removeObject:umodel];
    
    int replace_idx = arc4random() % models.count;
    UserModel *rmodel = models[replace_idx];
    [models replaceObjectAtIndex:replace_idx withObject:umodel ];
    [tempArray insertObject:rmodel atIndex:0];
}

//- (void)setupManagedObjectContext
//{
//    self.managedObjectContext = [[NSManagedObjectContext alloc] initWithConcurrencyType:NSMainQueueConcurrencyType];
//    self.managedObjectContext.persistentStoreCoordinator = [[NSPersistentStoreCoordinator alloc] initWithManagedObjectModel:self.managedObjectModel];
//    NSError* error;
//    [self.managedObjectContext.persistentStoreCoordinator addPersistentStoreWithType:NSSQLiteStoreType
//                                                                       configuration:nil
//                                                                                 URL:self.storeURL
//                                                                             options:nil
//                                                                               error:&error];
//    if (error) {
//        NSLog(@"error: %@", error);
//    }
//    self.managedObjectContext.undoManager = [[NSUndoManager alloc] init];
//}


@end

