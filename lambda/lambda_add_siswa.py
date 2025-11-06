import json
import boto3
import time
import os

dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table('BukuAbsenTable')

def handler(event, context):
    body = json.loads(event['body'])
    nama = body.get('nama')
    pesan = body.get('pesan', 'Telah Absen!')

    if not nama:
        return {
            'statusCode': 400,
            'body': json.dumps({'error': 'Nama wajib diisi!'})
        }

    item = {
        'id': str(int(time.time())),
        'nama': nama,
        'pesan': pesan
    }

    table.put_item(Item=item)
    return {
        'statusCode': 200,
        'headers': {'Access-Control-Allow-Origin': '*'},
        'body': json.dumps({'message': 'Data berhasil ditambahkan', 'item': item})
    }
