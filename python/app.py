from flask import Flask, render_template, request, redirect, url_for, session, flash
import requests

app = Flask(__name__)
app.secret_key = 'your_secret_key'  # Cambia con una chiave segreta sicura

backend_url = 'http://localhost:5000'  # Sostituisci con l'URL del tuo backend Go

@app.route('/')
def home():
    response = requests.get(f'{backend_url}/getGames')
    games = response.json()
    return render_template('home.html', games=games)

@app.route('/game/<string:gameTitle>')
def game(gameTitle):
    response = requests.get(f'{backend_url}/getGameByTitle/{gameTitle}')
    game = response.json()
    return render_template('game.html', game=game)

@app.route('/cart')
def cart():
    if 'cart' not in session:
        session['cart'] = []
    cart_items = session['cart']
    return render_template('cart.html', cart_items=cart_items)

@app.route('/add_to_cart/<string:gameTitle>')
def add_to_cart(gameTitle):
    if 'cart' not in session:
        session['cart'] = []
    session['cart'].append(gameTitle)
    return redirect(url_for('cart'))

@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        credentials = request.form
        response = requests.post(f'{backend_url}/login', json=credentials)
        if response.status_code == 200:
            session['user'] = response.json()['user']
            return redirect(url_for('home'))
        else:
            flash('Login Failed')
            return redirect(url_for('login'))
    return render_template('login.html')

@app.route('/signup', methods=['GET', 'POST'])
def signup():
    if request.method == 'POST':
        user_data = request.form
        response = requests.post(f'{backend_url}/signup', json=user_data)
        if response.status_code == 201:
            return redirect(url_for('login'))
        else:
            flash('Signup Failed')
            return redirect(url_for('signup'))
    return render_template('signup.html')

@app.route('/reviews/<string:gameTitle>', methods=['GET', 'POST'])
def reviews(gameTitle):
    if request.method == 'POST':
        review_data = request.form
        review_data['gameTitle'] = gameTitle
        response = requests.post(f'{backend_url}/addReview', json=review_data)
        return redirect(url_for('reviews', gameTitle=gameTitle))
    else:
        response = requests.get(f'{backend_url}/getReviewByGame/{gameTitle}')
        reviews = response.json()
        return render_template('reviews.html', reviews=reviews, gameTitle=gameTitle)

@app.route('/logout')
def logout():
    session.pop('user', None)
    return redirect(url_for('home'))

if __name__ == '__main__':
    app.run(debug=True)
