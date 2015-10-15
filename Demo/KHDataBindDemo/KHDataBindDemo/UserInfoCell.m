//
//  UserInfoCell.m
//  DataBindTest
//
//  Created by GevinChen on 2015/10/9.
//  Copyright © 2015年 GevinChen. All rights reserved.
//

#import "UserInfoCell.h"

@implementation UserInfoCell

- (void)awakeFromNib {
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void)onInit:(id)model
{
    
}

- (void)onLoad:(UserModel*)model
{
    self.nameLabel.text = [NSString stringWithFormat:@"%@ %@", model.user.name.first,model.user.name.last];
    self.genderLabel.text = model.user.gender;
    self.phoneLabel.text = model.user.phone;
    
    
    
}


@end
